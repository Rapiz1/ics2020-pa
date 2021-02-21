#include <common.h>
#include "syscall.h"

static Context* do_event(Event e, Context* c) {
  extern void do_syscall(Context *c);

  switch (e.event) {
    case EVENT_SYSCALL: Log("Received syscall event %d", c->GPR1); do_syscall(c); break;
    case EVENT_YIELD: Log("Received yield event"); break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
