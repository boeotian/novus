#include "internal/define_user_types.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/utilities.hpp"
#include "parse/nodes.hpp"
#include <unordered_map>
#include <unordered_set>

namespace frontend::internal {

auto defineType(
    Context* ctx,
    const TypeSubstitutionTable* typeSubTable,
    prog::sym::TypeId id,
    const parse::StructDeclStmtNode& n) -> bool {

  auto isValid    = true;
  auto fieldTable = prog::sym::FieldDeclTable{};
  for (const auto& field : n.getFields()) {
    // Get field type.
    const auto& fieldParseType = field.getType();
    const auto fieldTypeName   = getName(fieldParseType);
    const auto fieldType       = getOrInstType(ctx, typeSubTable, fieldParseType);
    if (!fieldType) {
      ctx->reportDiag(
          errUndeclaredType,
          getName(fieldParseType),
          fieldParseType.getParamCount(),
          fieldParseType.getSpan());
      isValid = false;
      continue;
    }

    // Get field identifier.
    const auto fieldName = getName(field.getIdentifier());
    if (fieldTable.lookup(fieldName)) {
      ctx->reportDiag(errDuplicateFieldNameInStruct, fieldName, field.getIdentifier().getSpan());
      isValid = false;
      continue;
    }
    if (typeSubTable != nullptr && typeSubTable->lookupType(fieldName)) {
      ctx->reportDiag(
          errFieldNameConflictsWithTypeSubstitution, fieldName, field.getIdentifier().getSpan());
      isValid = false;
      continue;
    }
    if (ctx->getProg()->lookupType(fieldName) || isReservedTypeName(fieldName)) {
      ctx->reportDiag(errFieldNameConflictsWithType, fieldName, field.getIdentifier().getSpan());
      isValid = false;
      continue;
    }
    fieldTable.registerField(fieldName, *fieldType);
  }

  if (isValid) {
    ctx->getProg()->defineStruct(id, std::move(fieldTable));
  }
  return isValid;
}

auto defineType(
    Context* ctx,
    const TypeSubstitutionTable* typeSubTable,
    prog::sym::TypeId id,
    const parse::UnionDeclStmtNode& n) -> bool {

  auto isValid = true;
  auto types   = std::vector<prog::sym::TypeId>{};
  for (const auto& parseType : n.getTypes()) {
    const auto type = getOrInstType(ctx, typeSubTable, parseType);
    if (!type) {
      ctx->reportDiag(
          errUndeclaredType, getName(parseType), parseType.getParamCount(), parseType.getSpan());
      isValid = false;
      continue;
    }
    if (std::find(types.begin(), types.end(), *type) != types.end()) {
      ctx->reportDiag(
          errDuplicateTypeInUnion,
          getName(parseType),
          getDisplayName(*ctx, *type),
          parseType.getSpan());
      isValid = false;
      continue;
    }
    types.push_back(*type);
  }

  if (isValid) {
    ctx->getProg()->defineUnion(id, std::move(types));
  }
  return isValid;
}

auto defineType(Context* ctx, prog::sym::TypeId id, const parse::EnumDeclStmtNode& n) -> bool {

  auto isValid      = true;
  auto values       = std::unordered_set<int32_t>{};
  auto entries      = std::unordered_map<std::string, int32_t>{};
  int32_t lastValue = -1;

  for (const auto& entry : n.getEntries()) {
    const auto name  = getName(entry.getIdentifier());
    const auto value = lastValue =
        entry.getValueSpec() ? entry.getValueSpec()->getValue() : lastValue + 1;

    if (!entries.insert({name, value}).second) {
      ctx->reportDiag(errDuplicateEntryNameInEnum, name, entry.getSpan());
      isValid = false;
    }
    if (!values.insert(value).second) {
      ctx->reportDiag(errDuplicateEntryValueInEnum, value, entry.getSpan());
      isValid = false;
    }
  }

  if (isValid) {
    ctx->getProg()->defineEnum(id, std::move(entries));
  }
  return isValid;
}

} // namespace frontend::internal
