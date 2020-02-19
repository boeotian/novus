#pragma once

namespace prog::sym {

enum class FuncKind {
  NoOp,
  User,

  AddInt,
  SubInt,
  MulInt,
  DivInt,
  RemInt,
  NegateInt,
  IncrementInt,
  DecrementInt,
  ShiftLeftInt,
  ShiftRightInt,
  AndInt,
  OrInt,
  XorInt,
  InvInt,
  CheckEqInt,
  CheckNEqInt,
  CheckLeInt,
  CheckLeEqInt,
  CheckGtInt,
  CheckGtEqInt,

  AddLong,
  SubLong,
  MulLong,
  DivLong,
  RemLong,
  NegateLong,
  IncrementLong,
  DecrementLong,
  CheckEqLong,
  CheckNEqLong,
  CheckLeLong,
  CheckLeEqLong,
  CheckGtLong,
  CheckGtEqLong,

  AddFloat,
  SubFloat,
  MulFloat,
  DivFloat,
  ModFloat,
  PowFloat,
  SqrtFloat,
  SinFloat,
  CosFloat,
  TanFloat,
  ASinFloat,
  ACosFloat,
  ATanFloat,
  ATan2Float,
  NegateFloat,
  IncrementFloat,
  DecrementFloat,
  CheckEqFloat,
  CheckNEqFloat,
  CheckLeFloat,
  CheckLeEqFloat,
  CheckGtFloat,
  CheckGtEqFloat,

  InvBool,
  CheckEqBool,
  CheckNEqBool,

  AddString,
  LengthString,
  IndexString,
  SliceString,
  CheckEqString,
  CheckNEqString,

  IncrementChar,
  DecrementChar,

  ConvIntFloat,
  ConvFloatInt,
  ConvIntString,
  ConvLongString,
  ConvFloatString,
  ConvBoolString,
  ConvCharString,
  ConvIntChar,

  DefInt,
  DefLong,
  DefFloat,
  DefBool,
  DefString,

  MakeStruct,
  MakeUnion,

  FutureWait,
  FutureBlock,

  CheckEqUserType,
  CheckNEqUserType,

  ActionConWriteChar,
  ActionConWriteString,
  ActionConWriteStringLine,
  ActionConReadChar,
  ActionConReadStringLine,
  ActionGetEnvArgCount,
  ActionGetEnvArg,
  ActionGetEnvVar,
  ActionSleep,
  ActionAssert,
};

} // namespace prog::sym
