#pragma once

#include <cstdint>

namespace OpenShock::JSON {
  enum class JsonValueType : uint8_t {
    Null,
    True,
    False,
    NumberInt,
    NumberFloat,
    String,
    Array,
    Object
  };
}  // namespace OpenShock::JSON
