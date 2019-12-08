#pragma once
#include "frontend/diag.hpp"
#include "frontend/source.hpp"
#include "parse/node_error.hpp"

namespace frontend {

[[nodiscard]] auto errParseError(const Source& src, const parse::ErrorNode& n) -> Diag;

[[nodiscard]] auto
errUnsupportedLiteral(const Source& src, const std::string& name, input::Span span) -> Diag;

[[nodiscard]] auto
errTypeAlreadyDeclared(const Source& src, const std::string& name, input::Span span) -> Diag;

[[nodiscard]] auto
errTypeNameConflictsWithFunc(const Source& src, const std::string& name, input::Span span) -> Diag;

[[nodiscard]] auto
errTypeNameConflictsWithAction(const Source& src, const std::string& name, input::Span span)
    -> Diag;

[[nodiscard]] auto
errDuplicateFieldNameInStruct(const Source& src, const std::string& fieldName, input::Span span)
    -> Diag;

[[nodiscard]] auto errCyclicStruct(
    const Source& src,
    const std::string& fieldName,
    const std::string& structName,
    input::Span span) -> Diag;

[[nodiscard]] auto
errFieldNameConflictsWithType(const Source& src, const std::string& name, input::Span span) -> Diag;

[[nodiscard]] auto errFieldNotFoundOnType(
    const Source& src, const std::string& fieldName, const std::string& typeName, input::Span span)
    -> Diag;

[[nodiscard]] auto
errDuplicateTypeInUnion(const Source& src, const std::string& typeName, input::Span span) -> Diag;

[[nodiscard]] auto errNonUnionIsExpression(const Source& src, input::Span span) -> Diag;

[[nodiscard]] auto errTypeNotPartOfUnion(
    const Source& src, const std::string& typeName, const std::string& unionName, input::Span span)
    -> Diag;

[[nodiscard]] auto errUncheckedIsExpressionWithConst(const Source& src, input::Span span) -> Diag;

[[nodiscard]] auto
errConvFuncCannotSpecifyReturnType(const Source& src, const std::string& name, input::Span span)
    -> Diag;

[[nodiscard]] auto
errFuncNameConflictsWithAction(const Source& src, const std::string& name, input::Span span)
    -> Diag;

[[nodiscard]] auto
errDuplicateFuncDeclaration(const Source& src, const std::string& name, input::Span span) -> Diag;

[[nodiscard]] auto
errUnableToInferFuncReturnType(const Source& src, const std::string& name, input::Span span)
    -> Diag;

[[nodiscard]] auto errNonMatchingFuncReturnType(
    const Source& src,
    const std::string& name,
    const std::string& declaredType,
    const std::string& returnedType,
    input::Span span) -> Diag;

[[nodiscard]] auto
errConstNameConflictsWithType(const Source& src, const std::string& name, input::Span span) -> Diag;

[[nodiscard]] auto
errConstNameConflictsWithFunction(const Source& src, const std::string& name, input::Span span)
    -> Diag;

[[nodiscard]] auto
errConstNameConflictsWithAction(const Source& src, const std::string& name, input::Span span)
    -> Diag;

[[nodiscard]] auto
errConstNameConflictsWithConst(const Source& src, const std::string& name, input::Span span)
    -> Diag;

[[nodiscard]] auto errUndeclaredType(const Source& src, const std::string& name, input::Span span)
    -> Diag;

[[nodiscard]] auto errUndeclaredConst(const Source& src, const std::string& name, input::Span span)
    -> Diag;

[[nodiscard]] auto
errUninitializedConst(const Source& src, const std::string& name, input::Span span) -> Diag;

[[nodiscard]] auto errUndeclaredFunc(const Source& src, const std::string& name, input::Span span)
    -> Diag;

[[nodiscard]] auto errUndeclaredFuncOverload(
    const Source& src,
    const std::string& name,
    const std::vector<std::string>& argTypes,
    input::Span span) -> Diag;

[[nodiscard]] auto errUndeclaredAction(const Source& src, const std::string& name, input::Span span)
    -> Diag;

[[nodiscard]] auto errUndeclaredActionOverload(
    const Source& src,
    const std::string& name,
    const std::vector<std::string>& argTypes,
    input::Span span) -> Diag;

[[nodiscard]] auto
errUnsupportedOperator(const Source& src, const std::string& name, input::Span span) -> Diag;

[[nodiscard]] auto errUndeclaredUnaryOperator(
    const Source& src, const std::string& name, const std::string& type, input::Span span) -> Diag;

[[nodiscard]] auto errUndeclaredBinOperator(
    const Source& src,
    const std::string& name,
    const std::string& lhsType,
    const std::string& rhsType,
    input::Span span) -> Diag;

[[nodiscard]] auto errBranchesHaveNoCommonType(const Source& src, input::Span span) -> Diag;

[[nodiscard]] auto errNoConversionFound(
    const Source& src, const std::string& from, const std::string& to, input::Span span) -> Diag;

[[nodiscard]] auto errNonExhaustiveSwitchWithoutElse(const Source& src, input::Span span) -> Diag;

} // namespace frontend
