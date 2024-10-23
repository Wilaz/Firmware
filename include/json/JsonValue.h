#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "JsonValueType.h"

namespace OpenShock::JSON {
  class JsonValue {
  public:
    JsonValue() noexcept;
    JsonValue(const JsonValue& other);
    JsonValue(JsonValue&& other) noexcept;
    JsonValue& operator=(const JsonValue& other);
    JsonValue& operator=(JsonValue&& other) noexcept;
    ~JsonValue();

    JsonValue(bool value) noexcept;
    JsonValue(int64_t value) noexcept;
    JsonValue(double value) noexcept;
    JsonValue(std::string_view value) noexcept;
    JsonValue(const char* value) noexcept;
    JsonValue(const std::string& value) noexcept;
    JsonValue(const std::vector<JsonValue>& value) noexcept;
    JsonValue(std::vector<JsonValue>&& value) noexcept;
    JsonValue(const std::vector<std::string_view>& keys, const std::vector<JsonValue>& values) noexcept;
    JsonValue(std::vector<std::string_view>&& keys, std::vector<JsonValue>&& values) noexcept;

    JsonValueType getType() const noexcept;
    bool isNull() const noexcept;
    bool isBoolean() const noexcept;
    bool isNumberInt() const noexcept;
    bool isNumberFloat() const noexcept;
    bool isString() const noexcept;
    bool isArray() const noexcept;
    bool isObject() const noexcept;

    bool getBoolean() const;
    int64_t getNumberInt() const;
    double getNumberFloat() const;
    std::string_view getString() const;
    const JsonValue& operator[](std::size_t index) const;
    const JsonValue& operator[](std::string_view key) const;

    std::size_t size() const;

    void push_back(const JsonValue& value);
    void push_back(JsonValue&& value);
    void insert(std::string_view key, const JsonValue& value);
    void insert(std::string_view key, JsonValue&& value);

    void clear();

    std::size_t subnodeCount() const;

  private:
    void* m_heap;
    void* m_node;
  };
}  // namespace OpenShock::JSON
