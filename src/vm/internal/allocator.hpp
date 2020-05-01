#pragma once
#include "gsl.hpp"
#include "internal/executor_registry.hpp"
#include "internal/garbage_collector.hpp"
#include "internal/likely.hpp"
#include <atomic>
#include <utility>

namespace vm::internal {

const auto gcByteInterval = 100 * 1024 * 1024; // 100 MiB

class FutureRef;
class LongRef;
class StreamRef;
class StringRef;
class StringLinkRef;
class StructRef;

class Allocator final {
public:
  explicit Allocator(ExecutorRegistry* execRegistry) noexcept;
  Allocator(const Allocator& rhs) = delete;
  Allocator(Allocator&& rhs)      = delete;
  ~Allocator() noexcept;

  auto operator=(const Allocator& rhs) -> Allocator& = delete;
  auto operator=(Allocator&& rhs) -> Allocator& = delete;

  // Allocate a string, upon failure returns {nullptr, nullptr}.
  [[nodiscard]] auto allocStr(unsigned int size) noexcept -> std::pair<StringRef*, uint8_t*>;

  // Allocate a string from a literal, upon failure returns {nullptr}.
  [[nodiscard]] auto allocStrLit(const std::string& literal) noexcept -> StringRef*;

  // Allocate a string-link, upon failure returns {nullptr}.
  [[nodiscard]] auto allocStrLink(Ref* prev, Value val) noexcept -> StringLinkRef*;

  // Allocate a struct, upon failure returns {nullptr, nullptr}.
  [[nodiscard]] auto allocStruct(uint8_t fieldCount) noexcept -> std::pair<StructRef*, Value*>;

  // Allocate a plain ref type, upon failure returns {nullptr}.
  template <typename RefType, class... ArgTypes>
  [[nodiscard]] auto allocPlain(ArgTypes&&... args) noexcept -> RefType* {
    static_assert(std::is_convertible<RefType*, Ref*>());

    auto mem = alloc<RefType>(0);
    if (unlikely(mem.first == nullptr)) {
      return nullptr;
    }

    auto* refPtr = static_cast<RefType*>(new (mem.first) RefType{std::forward<ArgTypes>(args)...});
    initRef(refPtr);
    return refPtr;
  }

  [[nodiscard]] inline auto getHeadAlloc() noexcept -> Ref* {
    return m_head.load(std::memory_order_acquire);
  }

  [[nodiscard]] inline auto getNextAlloc(Ref* ref) noexcept -> Ref* { return ref->m_next; }

  // Not thread-safe, should not be called concurrently.
  inline auto freeNext(Ref* ref) noexcept -> Ref* {
    auto* toFree = ref->m_next;
    if (toFree) {
      auto* next  = toFree->m_next;
      ref->m_next = next;
      freeUnsafe(toFree);
      return next;
    }
    return nullptr;
  }

private:
  GarbageCollector m_gc;
  std::atomic<Ref*> m_head;
  std::atomic<int> m_bytesUntilNextCollection;

  auto initRef(Ref* ref) noexcept -> void;

  template <typename ConcreteRef>
  auto alloc(const unsigned int payloadsize) noexcept -> std::pair<void*, void*> {
    // Make a single allocation of the header and the payload.
    const auto refSize   = sizeof(ConcreteRef);
    const auto allocSize = refSize + payloadsize;
    void* refPtr         = std::malloc(refSize + payloadsize); // NOLINT: Manual memory management.
    void* payloadPtr     = static_cast<char*>(refPtr) + refSize;

    // Note: malloc can fail and in that case this function will return {nullptr, nullptr}.

    // Keep track of how many bytes we are still allowed to allocate before running gc.
    if (unlikely(m_bytesUntilNextCollection.fetch_sub(allocSize, std::memory_order_acq_rel) < 0)) {
      m_bytesUntilNextCollection.store(gcByteInterval, std::memory_order_release);
      m_gc.requestCollection();
    }

    return {refPtr, payloadPtr};
  }

  // Destruct and free the given ref, unsafe because it doesn't update any of the bookkeeping.
  inline static auto freeUnsafe(Ref* ref) noexcept -> void {
    // Call the (virtual) destructor.
    ref->~Ref();
    // Free the backing memory.
    std::free(ref); // NOLINT: Manual memory management.
  }
};

} // namespace vm::internal
