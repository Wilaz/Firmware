#include "event_handlers/impl/WSGateway.h"

const char* const TAG = "ServerMessageHandlers";

#include "CommandHandler.h"
#include "Logging.h"
#include "ShockerModelType.h"

#include <cstdint>

using namespace OpenShock::MessageHandlers::Server;

void _Private::HandleShockerCommandList(const OpenShock::Serialization::Gateway::GatewayToHubMessage* root) {
  auto msg = root->payload_as_ShockerCommandList();
  if (msg == nullptr) {
    OS_LOGE(TAG, "Payload cannot be parsed as ShockerCommandList");
    return;
  }

  auto commands = msg->commands();
  if (commands == nullptr) {
    OS_LOGE(TAG, "Received invalid command list from API");
    return;
  }

  OS_LOGV(TAG, "Received command list from API (%u commands)", commands->size());

  for (auto command : *commands) {
    uint16_t id                   = command->id();
    uint8_t intensity             = command->intensity();
    uint16_t durationMs           = command->duration();
    OpenShock::ShockerModelType model  = command->model();
    OpenShock::ShockerCommandType type = command->type();

    const char* modelStr = OpenShock::Serialization::Types::EnumNameShockerModelType(model);
    const char* typeStr  = OpenShock::Serialization::Types::EnumNameShockerCommandType(type);

    OS_LOGV(TAG, "   ID %u, Intensity %u, Duration %u, Model %s, Type %s", id, intensity, durationMs, modelStr, typeStr);

    if (!OpenShock::CommandHandler::HandleCommand(model, id, type, intensity, durationMs)) {
      OS_LOGE(TAG, "Remote command failed/rejected!");
    }
  }
}
