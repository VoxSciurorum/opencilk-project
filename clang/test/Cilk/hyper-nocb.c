// RUN: %clang_cc1 %s -x c -fopencilk -verify -emit-llvm -disable-llvm-passes -o /dev/null
// Compiling this file should not crash.

extern int _Hyperobject x;
// expected-error@-1{{view type must be a class}}

void function1()
{
  int _Hyperobject y = 1;
  // expected-error@-1{{view type must be a class}}
  (void)x; (void)y;
  // expected-error@-1{{call to undeclared library function}}
  // expected-note@-2{{include the header <cilk/reducer>}}
  // Ideally the error message should refer to reducer lookup
  // instead of a call to a function not named in user code.
}
