#include "json/JsonValue.h"

#include "Common.h"
#include "json/JsonString.h"

struct JsonNode {
  OpenShock::JSON::JsonValueType type;
  union {
    int64_t asNumberInt;
    double asNumberFloat;
    struct {
      std::uint16_t offs;  // Offset into heap strings
    } asString;
    struct {
      std::uint16_t offs;  // Offset into heap nodes
      std::uint16_t size;  // Number of elements
    } asArray;
    struct {
      std::uint16_t keysOffs;   // Offset into heap strings
      std::uint16_t nodesOffs;  // Offset into heap nodes
      std::uint16_t size;       // Number of elements
    } asObject;
  } data;
};

struct JsonHeap {
  std::vector<OpenShock::JSON::JsonString> strings;
  std::vector<JsonNode> nodes;
};

using namespace OpenShock::JSON;

JsonValue::JsonValue() noexcept
  : m_heap(nullptr)
  , m_node(nullptr)
{
}

JsonValue::JsonValue(const JsonValue& other)
  : m_heap(nullptr)
  , m_node(nullptr)
{
  if (other.m_heap != nullptr && other.m_node != nullptr) {
    m_heap = other.m_heap;
    m_node = other.m_node;
  }
}

JsonValue::JsonValue(JsonValue&& other) noexcept
  : m_heap(nullptr)
  , m_node(nullptr)
{
  if (other.m_heap != nullptr && other.m_node != nullptr) {
    m_heap = other.m_heap;
    m_node = other.m_node;

    other.m_heap = nullptr;
    other.m_node = nullptr;
  }
}

JsonValue& JsonValue::operator=(const JsonValue& other)
{
  if (this != &other) {
    if (m_heap != nullptr && m_node != nullptr) {
      m_heap = nullptr;
      m_node = nullptr;
    }

    if (other.m_heap != nullptr && other.m_node != nullptr) {
      m_heap = other.m_heap;
      m_node = other.m_node;
    }
  }

  return *this;
}

JsonValue& JsonValue::operator=(JsonValue&& other) noexcept
{
  if (this != &other) {
    if (m_heap != nullptr && m_node != nullptr) {
      m_heap = nullptr;
      m_node = nullptr;
    }

    if (other.m_heap != nullptr && other.m_node != nullptr) {
      m_heap = other.m_heap;
      m_node = other.m_node;

      other.m_heap = nullptr;
      other.m_node = nullptr;
    }
  }

  return *this;
}

JsonValue::~JsonValue()
{
  if (m_heap != nullptr && m_node != nullptr) {
    delete reinterpret_cast<JsonHeap*>(m_heap);
    m_heap = nullptr;
  }
}

std::size_t json_getSubnodeCount(const JsonHeap* heap, const JsonNode* node)
{
  if (heap == nullptr || node == nullptr) {
    return 0;
  }

  std::size_t offset;
  std::size_t size;

  switch (node->type) {
    case JsonValueType::Array:
    {
      offset = node->data.asArray.offs;
      size   = node->data.asArray.size;
      break;
    }
    case JsonValueType::Object:
    {
      offset = node->data.asObject.nodesOffs;
      size   = node->data.asObject.size;
      break;
    }
    default:
      return 0;
  }

  std::size_t count = size;
  for (std::uint16_t i = 0; i < size; i++) {
    count += json_getSubnodeCount(heap, &heap->nodes[offset + i]);
  }

  return count;
}

std::size_t JsonValue::subnodeCount() const
{
  return json_getSubnodeCount(reinterpret_cast<const JsonHeap*>(m_heap), reinterpret_cast<const JsonNode*>(m_node));
}
