#include <common.h>
#include "syscall.h"
#include <errno.h>

static void sys_exit(Context *c) {
  halt(0);
}

static void sys_yield(Context *c) {
  yield();
  c->GPRx = 0;
}

static void sys_write(Context *c) {
  int fd = c->GPR2;
  char *buf = (char*)c->GPR3;
  size_t count = c->GPR4;
  if (fd == 1 || fd == 2) {
    for (int i = 0; i < count; i++)
      putch(buf[i]);
    c->GPRx = count;
  } else {
    c->GPRx = -EBADF;
  }
}

static void sys_brk(Context *c) {
  c->GPRx = 0;
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    case SYS_exit: sys_exit(c); break;
    case SYS_yield: sys_yield(c); break;
    case SYS_write: sys_write(c); break;
    case SYS_brk: sys_brk(c); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
