// RUN: %clang_cc1 %s -x c -fopencilk -verify -emit-llvm -disable-llvm-passes -o /dev/null
// Compiling this file should not crash.

extern int _Hyperobject x;
// expected-error@-1{{view type must be a class}}

void function1()
{
  int _Hyperobject y = 1;
  // expected-error@-1{{view type must be a class}}
  (void)x;
  ++x;
  y++;
  (void)y;
}
