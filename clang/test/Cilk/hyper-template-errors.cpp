// RUN: %clang_cc1 %s -fopencilk -verify -fsyntax-only

extern void identity(void *), reduce(void *, void *);

template<typename VIEW>
struct reducer
{
  static void identity(void *);
  static void reduce(void *, void *);
// See SemaType.cpp:ContainsHyperobject for choice of error message.
  VIEW _Hyperobject(identity, reduce) value1;
 // expected-error@-1{{type 'long _Hyperobject(identity, reduce)', which contains a hyperobject, may not be a hyperobject}}
 // expected-error@-2{{type 'reducer<char>', which contains a hyperobject, may not be a hyperobject}}
 // expected-error@-3{{type 'wrap<int _Hyperobject(::identity, ::reduce)>', which contains a hyperobject, may not be a hyperobject}}
  int _Hyperobject(::identity, ::reduce) value2 = 0;
  reducer();
};

reducer<long _Hyperobject(identity, reduce)> r_hl; // expected-note{{in instantiation}}
reducer<char> r_l;
reducer<int[2]> r_i2;

int f() { return r_l.value1 + r_l.value2; }
int g() { return r_i2.value1[0]; }

reducer<reducer<char>> s; // expected-note{{in instantiation}}

template<typename T> struct wrap { T field; };
reducer<wrap<int _Hyperobject(identity, reduce)>> t; // expected-note{{in instantiation}}

template<typename V> struct T { int a; };
// expected-note@-1{{previous definition}}
// expected-note@-2{{t}}
// expected-note@-3{{t}}
// expected-note@-4{{t}}
// expected-note@-5{{t}}
struct T { int b; };
// expected-error@-1{{redefinition of 'T' as different kind of symbol}}
// This tests for a crash trying to deduce the view type T of the hyperobject.
T cilk_reducer(identity, reduce) t;
// expected-error@-1{{deduced class template specialization type}}
// expected-error@-2{{deduction of template arguments}}
