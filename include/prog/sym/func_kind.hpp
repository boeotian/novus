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

  CombineChar,
  AppendChar,
  IncrementChar,
  DecrementChar,

  ConvIntLong,
  ConvIntFloat,
  ConvLongInt,
  ConvFloatInt,
  ConvIntString,
  ConvLongString,
  ConvFloatString,
  ConvBoolString,
  ConvCharString,
  ConvIntChar,
  ConvFloatChar,

  DefInt,
  DefLong,
  DefFloat,
  DefBool,
  DefString,

  MakeStruct,
  MakeUnion,

  FutureWaitNano,
  FutureBlock,

  CheckEqUserType,
  CheckNEqUserType,

  ActionConWriteChar,
  ActionConWriteString,
  ActionConWriteStringLine,

  ActionConReadChar,
  ActionConReadStringLine,

  ActionStreamOpenFile,
  ActionStreamCheckValid,
  ActionStreamRead,
  ActionStreamWrite,
  ActionStreamFlush,
  ActionFileRemove,

  ActionGetEnvArgCount,
  ActionGetEnvArg,
  ActionGetEnvVar,

  ActionClockMicroSinceEpoch,
  ActionClockNanoSteady,

  ActionSleepNano,
  ActionAssert,
};

} // namespace prog::sym
