#pragma once

#include "wifi/WiFiScanStatus.h"

#include <esp_wifi_types.h>

#include <cstdint>
#include <functional>

namespace OpenShock::WiFiScanManager {
  bool Init();

  bool IsScanning();

  bool StartScan();
  bool AbortScan();

  typedef std::function<void(OpenShock::WiFiScanStatus)> StatusChangedHandler;
  typedef std::function<void(const std::vector<const wifi_ap_record_t*>& networkRecords)> NetworksDiscoveredHandler;

  uint64_t RegisterStatusChangedHandler(const StatusChangedHandler& handler);
  void UnregisterStatusChangedHandler(uint64_t id);

  uint64_t RegisterNetworksDiscoveredHandler(const NetworksDiscoveredHandler& handler);
  void UnregisterNetworksDiscoveredHandler(uint64_t id);
}  // namespace OpenShock::WiFiScanManager
