#include "delegate_table.hpp"
#include "utilities.hpp"

namespace frontend::internal {

static auto getName(Context* context, const prog::sym::TypeSet& input, prog::sym::TypeId output)
    -> std::string {
  auto result = std::string{"__func"};
  for (const auto& type : input) {
    const auto& typeName = context->getProg()->getTypeDecl(type).getName();
    result += '_' + typeName;
  }
  result += '_' + context->getProg()->getTypeDecl(output).getName();
  return result;
}

auto DelegateTable::Hasher::operator()(const signature& id) const -> std::size_t {
  std::size_t result = id.first.getCount();
  for (auto& i : id.first) {
    result ^= i.getNum() + 0x9e3779b9 + (result << 6U) + (result >> 2U); // NOLINT: Magic numbers
  }
  result ^=
      id.second.getNum() + 0x9e3779b9 + (result << 6U) + (result >> 2U); // NOLINT: Magic numbers
  return result;
}

auto DelegateTable::getDelegate(Context* context, const prog::sym::TypeSet& types)
    -> prog::sym::TypeId {
  auto inputTypes = std::vector<prog::sym::TypeId>{};
  for (auto i = 0U; i != types.getCount() - 1; ++i) {
    inputTypes.push_back(types[i]);
  }
  auto outputType = types[types.getCount() - 1];
  return getDelegate(context, prog::sym::TypeSet{std::move(inputTypes)}, outputType);
}

auto DelegateTable::getDelegate(
    Context* context, const prog::sym::TypeSet& input, prog::sym::TypeId output)
    -> prog::sym::TypeId {
  auto sig = std::make_pair(input, output);

  // Try to find an existing delegate with the same signature.
  auto itr = m_delegates.find(sig);
  if (itr != m_delegates.end()) {
    return itr->second;
  }

  // Declare a new delegate.
  const auto delegateType =
      context->getProg()->declareUserDelegate(getName(context, input, output));

  // Define the delegate.
  context->getProg()->defineUserDelegate(delegateType, input, output);

  // Keep track of some extra information about the type.
  auto types = std::vector<prog::sym::TypeId>{};
  types.insert(types.end(), input.begin(), input.end());
  types.push_back(output);
  context->declareTypeInfo(
      delegateType, TypeInfo{"func", input::Span{0}, prog::sym::TypeSet{std::move(types)}});

  m_delegates.insert({std::move(sig), delegateType});
  return delegateType;
}

} // namespace frontend::internal