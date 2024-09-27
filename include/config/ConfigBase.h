#pragma once

#include "serialization/_fbs/HubConfig_generated.h"

#include <cJSON.h>

namespace OpenShock::Config {
  template<typename T>
  struct ConfigBase {
    virtual void ToDefault() = 0;

    virtual bool FromFlatbuffers(const T* config)                                                                                     = 0;
    [[nodiscard]] virtual flatbuffers::Offset<T> ToFlatbuffers(flatbuffers::FlatBufferBuilder& builder, bool withSensitiveData) const = 0;

    virtual bool FromJSON(const cJSON* json)                          = 0;
    [[nodiscard]] virtual cJSON* ToJSON(bool withSensitiveData) const = 0;
  };

}  // namespace OpenShock::Config
