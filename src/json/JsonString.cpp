#include "json/JsonString.h"

#include <cstring>

using namespace OpenShock::JSON;

JsonString JsonString::Create(std::string_view value)
{
  JsonString str;
  str.copy(value);

  return str;
}

JsonString JsonString::CreateRef(std::string_view value)
{
  return JsonString(value);
}

JsonString::JsonString() noexcept
  : m_data(nullptr)
  , m_size(0)
  , m_owned(false)
{
}

JsonString::JsonString(const JsonString& other)
  : m_data(nullptr)
  , m_size(0)
  , m_owned(false)
{
  if (other.m_data != nullptr) {
    m_data  = new char[other.m_size];
    m_size  = other.m_size;
    m_owned = true;

    memcpy(const_cast<char*>(m_data), other.m_data, other.m_size);
  }
}

JsonString::JsonString(JsonString&& other) noexcept
  : m_data(nullptr)
  , m_size(0)
  , m_owned(false)
{
  if (other.m_data != nullptr) {
    m_data  = other.m_data;
    m_size  = other.m_size;
    m_owned = other.m_owned;

    other.m_data  = nullptr;
    other.m_size  = 0;
    other.m_owned = false;
  }
}

JsonString::JsonString(std::string_view value)
  : m_data(nullptr)
  , m_size(0)
  , m_owned(false)
{
  if (!value.empty()) {
    m_data  = value.data();
    m_size  = value.size();
    m_owned = false;
  }
}

JsonString::~JsonString()
{
  clear();
}

void JsonString::copy(std::string_view value)
{
  clear();

  if (!value.empty()) {
    m_data  = new char[value.size()];
    m_size  = value.size();
    m_owned = true;

    memcpy(const_cast<char*>(m_data), value.data(), value.size());
  }
}

void JsonString::clear()
{
  if (m_owned && m_data != nullptr) {
    delete[] m_data;
    m_data = nullptr;
  }

  m_size  = 0;
  m_owned = false;
}

JsonString& JsonString::operator=(const JsonString& other)
{
  if (this != &other) {
    clear();

    if (other.m_data != nullptr) {
      m_data  = new char[other.m_size];
      m_size  = other.m_size;
      m_owned = true;

      memcpy(const_cast<char*>(m_data), other.m_data, other.m_size);
    }
  }

  return *this;
}

JsonString& JsonString::operator=(JsonString&& other) noexcept
{
  if (this != &other) {
    clear();

    if (other.m_data != nullptr) {
      m_data  = other.m_data;
      m_size  = other.m_size;
      m_owned = other.m_owned;

      other.m_data  = nullptr;
      other.m_size  = 0;
      other.m_owned = false;
    }
  }

  return *this;
}

JsonString& JsonString::operator=(std::string_view value)
{
  clear();

  if (!value.empty()) {
    m_data  = value.data();
    m_size  = value.size();
    m_owned = false;
  }

  return *this;
}
