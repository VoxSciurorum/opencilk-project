// RUN: %clang_cc1 %s --std=c++20 -x c++ -fopencilk -verify -emit-llvm -disable-llvm-passes -o - | FileCheck %s
// expected-no-diagnostics

extern void* operator new(unsigned long size, void *ptr);

struct __reducer_base {
  virtual ~__reducer_base() { }
  // TODO: Should size be a data member?
  virtual unsigned long size() = 0;
  virtual void identity(void *view) = 0;
  virtual void reduce(__reducer_base *l, __reducer_base *r) = 0;
};

extern "C" void *__hyper_lookup_0(__reducer_base *)
  __attribute__((nonnull, returns_nonnull));

struct R : public __reducer_base {
  int field;
  void identity(void *view) override
    __attribute__((nonnull));
  void reduce(__reducer_base *l, __reducer_base *r) override
    __attribute__((nonnull));
  unsigned long size() override;
};

void R::identity(void *view)
{
  ::new (view) (R);
}

void R::reduce(__reducer_base *l, __reducer_base *r)
{
  static_cast<R *>(l)->field *= static_cast<R *>(r)->field;
  static_cast<R *>(r)->~R();
}

unsigned long R::size()
{
  return sizeof *this;
}

struct R cilk_reducer r;
int lookup_r()
{
  return r.field;
}

// Virtual base class.
struct Distant : virtual __reducer_base
{
  long field;
  virtual void identity(void *view) override;
  virtual void reduce(__reducer_base *l, __reducer_base *r) override;
  unsigned long size() override { return sizeof *this; }
};
struct Distant cilk_reducer d;
// CHECK-LABEL: @_Z8lookup_dv
long lookup_d()
{
  // Currently the clang 19 front end generates an if (false) test
  // checking for a null pointer.  The value %cast.result is a PHI
  // of the unreachable block and the original pointer.
  // CHECK: call ptr @llvm.hyper.lookup.0(ptr %cast.result)
  return d.field;
}

// CHECK-LABEL: @_Z12lookup_d_ptrPH7Distant
long lookup_d_ptr(struct Distant cilk_reducer *d)
{
  // CHECK: call ptr @llvm.hyper.lookup.0(ptr %cast.result)
  return d->field;
}
