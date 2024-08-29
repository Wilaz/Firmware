#pragma once

#include "ShockerModelType.h"

#include <cJSON.h>

#include <cstdint>
#include <string>
#include <vector>

namespace OpenShock::Serialization::JsonAPI {
  struct LcgInstanceDetailsResponse {
    std::string name;
    std::string version;
    std::string currentTime;
    std::string countryCode;
    std::string fqdn;
  };
  struct BackendVersionResponse {
    std::string version;
    std::string commit;
    std::string currentTime;
  };
  struct AccountLinkResponse {
    std::string authToken;
  };
  struct DeviceInfoResponse {
    std::string deviceId;
    std::string deviceName;
    struct ShockerInfo {
      std::string id;
      uint16_t rfId;
      OpenShock::ShockerModelType model;
    };
    std::vector<ShockerInfo> shockers;
  };
  struct AssignLcgResponse {
    std::string fqdn;
    std::string country;
  };

  bool ParseLcgInstanceDetailsJsonResponse(int code, const cJSON* root, LcgInstanceDetailsResponse& out);
  bool ParseBackendVersionJsonResponse(int code, const cJSON* root, BackendVersionResponse& out);
  bool ParseAccountLinkJsonResponse(int code, const cJSON* root, AccountLinkResponse& out);
  bool ParseDeviceInfoJsonResponse(int code, const cJSON* root, DeviceInfoResponse& out);
  bool ParseAssignLcgJsonResponse(int code, const cJSON* root, AssignLcgResponse& out);
}  // namespace OpenShock::Serialization::JsonAPI
