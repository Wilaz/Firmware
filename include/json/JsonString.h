#pragma once

#include <cstdint>
#include <string_view>

namespace OpenShock::JSON {
  class JsonString {
  public:
    static JsonString Create(std::string_view value);
    static JsonString CreateRef(std::string_view value);

    JsonString() noexcept;
    JsonString(const JsonString& other);
    JsonString(JsonString&& other) noexcept;
    JsonString(std::string_view value);
    ~JsonString();

    void copy(std::string_view value);

    constexpr const char* data() const noexcept { return m_data; }
    constexpr std::uint16_t size() const noexcept { return m_size; }
    constexpr std::string_view view() const noexcept { return std::string_view(m_data, m_size); }

    void clear();

    JsonString& operator=(const JsonString& other);
    JsonString& operator=(JsonString&& other) noexcept;
    JsonString& operator=(std::string_view value);

  private:
    const char* m_data;
    std::uint16_t m_size;
    bool m_owned;
  };
  const std::size_t JsonStringSize = sizeof(JsonString);
}  // namespace OpenShock::JSON
