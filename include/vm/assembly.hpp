#pragma once
#include <vector>

namespace vm {

class Assembly final {
public:
  using entryPointIterator = typename std::vector<uint32_t>::const_iterator;

  Assembly(std::vector<uint8_t> instructions, std::vector<uint32_t> entryPoints);
  Assembly(const Assembly& rhs)     = delete;
  Assembly(Assembly&& rhs) noexcept = default;
  ~Assembly()                       = default;

  auto operator=(const Assembly& rhs) -> Assembly& = delete;
  auto operator=(Assembly&& rhs) noexcept -> Assembly& = delete;

  auto operator==(const Assembly& rhs) const noexcept -> bool;
  auto operator!=(const Assembly& rhs) const noexcept -> bool;

  [[nodiscard]] auto beginEntryPoints() const noexcept -> entryPointIterator;
  [[nodiscard]] auto endEntryPoints() const noexcept -> entryPointIterator;

  [[nodiscard]] auto getInstructions() const noexcept -> const std::vector<uint8_t>&;
  [[nodiscard]] auto getIp(uint32_t ipOffset) const noexcept -> const uint8_t*;

  [[nodiscard]] auto readInt32(uint32_t ipOffset) const noexcept -> int32_t;
  [[nodiscard]] auto readUInt32(uint32_t ipOffset) const noexcept -> uint32_t;

private:
  std::vector<uint8_t> m_instructions;
  std::vector<uint32_t> m_entryPoints;
};

} // namespace vm
