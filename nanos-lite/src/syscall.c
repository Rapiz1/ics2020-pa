#include <common.h>
#include "syscall.h"

static void SYS_yield(Context *c) {
  yield();
  c->GPRx = 0;
}

static void SYS_exit(Context *c) {
  halt(0);
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    case 0: SYS_exit(c); break;
    case 1: SYS_yield(c); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
