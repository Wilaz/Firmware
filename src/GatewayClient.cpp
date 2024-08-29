#include "GatewayClient.h"

#include "Common.h"
#include "config/Config.h"
#include "event_handlers/WebSocket.h"
#include "Logging.h"
#include "OtaUpdateManager.h"
#include "serialization/WSGateway.h"
#include "Time.h"
#include "util/CertificateUtils.h"
#include "VisualStateManager.h"

const char* const TAG = "GatewayClient";

using namespace OpenShock;

static bool s_bootStatusSent = false;

GatewayClient::GatewayClient(const std::string& authToken) : m_webSocket(), m_lastKeepAlive(0), m_state(State::Disconnected) {
  ESP_LOGD(TAG, "Creating GatewayClient");

  std::string headers = "Firmware-Version: " OPENSHOCK_FW_VERSION "\r\n"
                        "Device-Token: "
                      + authToken;

  m_webSocket.setUserAgent(OpenShock::Constants::FW_USERAGENT);
  m_webSocket.setExtraHeaders(headers.c_str());
  m_webSocket.onEvent(std::bind(&GatewayClient::_handleEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}
GatewayClient::~GatewayClient() {
  ESP_LOGD(TAG, "Destroying GatewayClient");
  m_webSocket.disconnect();
}

void GatewayClient::connect(const char* lcgFqdn) {
  if (m_state != State::Disconnected) {
    return;
  }

  _setState(State::Connecting);

//
//  ######  ########  ######  ##     ## ########  #### ######## ##    ##    ########  ####  ######  ##    ##
// ##    ## ##       ##    ## ##     ## ##     ##  ##     ##     ##  ##     ##     ##  ##  ##    ## ##   ##
// ##       ##       ##       ##     ## ##     ##  ##     ##      ####      ##     ##  ##  ##       ##  ##
//  ######  ######   ##       ##     ## ########   ##     ##       ##       ########   ##   ######  #####
//       ## ##       ##       ##     ## ##   ##    ##     ##       ##       ##   ##    ##        ## ##  ##
// ##    ## ##       ##    ## ##     ## ##    ##   ##     ##       ##       ##    ##   ##  ##    ## ##   ##
//  ######  ########  ######   #######  ##     ## ####    ##       ##       ##     ## ####  ######  ##    ##
//
// TODO: Implement certificate verification
//
#warning SSL certificate verification is currently not implemented, by RFC definition this is a security risk, and allows for MITM attacks, but the realistic risk is low

  m_webSocket.beginSSL(lcgFqdn, 443, "/1/ws/device");
  ESP_LOGW(TAG, "WEBSOCKET CONNECTION BY RFC DEFINITION IS INSECURE, remote endpoint can not be verified due to lack of CA verification support, theoretically this is a security risk and allows for MITM attacks, but the realistic risk is low");
}

void GatewayClient::disconnect() {
  if (m_state != State::Connected) {
    return;
  }
  _setState(State::Disconnecting);
  m_webSocket.disconnect();
}

bool GatewayClient::sendMessageTXT(StringView data) {
  if (m_state != State::Connected) {
    return false;
  }

  return m_webSocket.sendTXT(data.data(), data.length());
}

bool GatewayClient::sendMessageBIN(const uint8_t* data, std::size_t length) {
  if (m_state != State::Connected) {
    return false;
  }

  return m_webSocket.sendBIN(data, length);
}

bool GatewayClient::loop() {
  if (m_state == State::Disconnected) {
    return false;
  }

  m_webSocket.loop();

  // We are still in the process of connecting or disconnecting
  if (m_state != State::Connected) {
    // return true to indicate that we are still busy
    return true;
  }

  int64_t msNow = OpenShock::millis();

  int64_t timeSinceLastKA = msNow - m_lastKeepAlive;

  if (timeSinceLastKA >= 15'000) {
    _sendKeepAlive();
    m_lastKeepAlive = msNow;
  }

  return true;
}

void GatewayClient::_setState(State state) {
  if (m_state == state) {
    return;
  }

  m_state = state;

  switch (m_state) {
    case State::Disconnected:
      ESP_LOGI(TAG, "Disconnected from API");
      OpenShock::VisualStateManager::SetWebSocketConnected(false);
      break;
    case State::Connected:
      ESP_LOGI(TAG, "Connected to API");
      OpenShock::VisualStateManager::SetWebSocketConnected(true);
      break;
    default:
      break;
  }
}

void GatewayClient::_sendKeepAlive() {
  ESP_LOGV(TAG, "Sending Gateway keep-alive message");
  Serialization::Gateway::SerializeKeepAliveMessage([this](const uint8_t* data, std::size_t len) { return m_webSocket.sendBIN(data, len); });
}

void GatewayClient::_sendBootStatus() {
  if (s_bootStatusSent) return;

  ESP_LOGV(TAG, "Sending Gateway boot status message");

  int32_t updateId;
  if (!Config::GetOtaUpdateId(updateId)) {
    ESP_LOGE(TAG, "Failed to get OTA update ID");
    return;
  }

  OpenShock::OtaUpdateStep updateStep;
  if (!Config::GetOtaUpdateStep(updateStep)) {
    ESP_LOGE(TAG, "Failed to get OTA firmware boot type");
    return;
  }

  OpenShock::SemVer version;
  if (!OpenShock::TryParseSemVer(OPENSHOCK_FW_VERSION, version)) {
    ESP_LOGE(TAG, "Failed to parse firmware version");
    return;
  }

  s_bootStatusSent = Serialization::Gateway::SerializeBootStatusMessage(updateId, OtaUpdateManager::GetFirmwareBootType(), version, [this](const uint8_t* data, std::size_t len) { return m_webSocket.sendBIN(data, len); });

  if (s_bootStatusSent && updateStep != OpenShock::OtaUpdateStep::None) {
    if (!Config::SetOtaUpdateStep(OpenShock::OtaUpdateStep::None)) {
      ESP_LOGE(TAG, "Failed to reset firmware boot type to normal");
    }
  }
}

void GatewayClient::_handleEvent(WStype_t type, uint8_t* payload, std::size_t length) {
  (void)payload;

  switch (type) {
    case WStype_DISCONNECTED:
      _setState(State::Disconnected);
      break;
    case WStype_CONNECTED:
      _setState(State::Connected);
      _sendKeepAlive();
      _sendBootStatus();
      break;
    case WStype_TEXT:
      ESP_LOGW(TAG, "Received text from API, JSON parsing is not supported anymore :D");
      break;
    case WStype_ERROR:
      ESP_LOGE(TAG, "Received error from API");
      break;
    case WStype_FRAGMENT_TEXT_START:
      ESP_LOGD(TAG, "Received fragment text start from API");
      break;
    case WStype_FRAGMENT:
      ESP_LOGD(TAG, "Received fragment from API");
      break;
    case WStype_FRAGMENT_FIN:
      ESP_LOGD(TAG, "Received fragment fin from API");
      break;
    case WStype_PING:
      ESP_LOGD(TAG, "Received ping from API");
      break;
    case WStype_PONG:
      ESP_LOGD(TAG, "Received pong from API");
      break;
    case WStype_BIN:
      EventHandlers::WebSocket::HandleGatewayBinary(payload, length);
      break;
    case WStype_FRAGMENT_BIN_START:
      ESP_LOGE(TAG, "Received binary fragment start from API, this is not supported!");
      break;
    default:
      ESP_LOGE(TAG, "Received unknown event from API");
      break;
  }
}
