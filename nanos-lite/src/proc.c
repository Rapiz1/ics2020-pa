#include <proc.h>
#include <common.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb() { current = &pcb_boot; }

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite with arg '%d' for the %dth time!, at %d",
        (uintptr_t)arg, j, &arg);
    j++;
    yield();
  }
}

void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]) {
  // We must copy str before calling loader.
  // Otherwise the string may be overwritten.

  //void *st = heap.end, *strp = heap.end;
  void *st_pg = new_page(8);
  void *st = st_pg + PGSIZE*8, *strp = st;

  int envc = 0;
  for(int i = 0; envp && envp[i]; i++) {
    envc++;
    int n = strlen(envp[i]) + 1;
    st -= n;
    strcpy(st, envp[i]);
  }

  int argc = 0;
  for(int i = 0; argv && argv[i]; i++) {
    argc++;
    int n = strlen(argv[i]) + 1;
    st -= n;
    strcpy(st, argv[i]);
  }

  st -= sizeof(char*)*(envc+1);
  char **user_envp = st;
  for (int i = 0; i < envc; i++) {
    int n = strlen(envp[i]) + 1;
    strp -= n;
    user_envp[i] = strp;
  }
  user_envp[envc] = NULL;

  st -= sizeof(char*)*(argc+1);
  char **user_argv = st;
  for (int i = 0; i < argc; i++) {
    int n = strlen(argv[i]) + 1;
    strp -= n;
    user_argv[i] = strp;
  }
  user_argv[argc] = NULL;

  st -= sizeof(uint32_t);
  *(uint32_t*)st = argc;

  protect(&pcb->as);
  map(&pcb->as, pcb->as.area.end - 8*PGSIZE, st_pg, 0);

  extern uintptr_t loader(PCB *pcb, const char *filename);

  uintptr_t entry = loader(pcb, filename);

  assert(entry);

  Context *cp = ucontext(&pcb->as, RANGE(pcb, pcb+1), (void(*)())entry);
  pcb->cp = cp;

  cp->GPRx = (uint32_t)((st - st_pg) + pcb->as.area.end - 8*PGSIZE);
  printf("stack vaddr %d, paddr %d(start from %d)", cp->GPRx, st, st_pg);
}

void context_kload(PCB *pcb, void (*entry)(void *), void *arg) {
  Context *cp = kcontext(RANGE(pcb, pcb + 1), entry, arg);
  pcb->cp = cp;
}

void init_proc() {
  //context_kload(&pcb[0], hello_fun, (void*)1);
  char *argv[] = {"/bin/menu", "HELLO FROM NANOS-LITE", NULL};
  char *envp[] = {"HELLO=NANOS-LITE", "AUTHOR=rapiz", NULL};
  //context_uload(&pcb[1], "/bin/menu", argv, envp);
  context_uload(&pcb[0], "/bin/dummy", argv, envp);
  switch_boot_pcb();

  Log("Initializing processes...");

  // load program here
  /*
  extern void naive_uload(PCB *pcb, const char *filename);
  naive_uload(NULL, "/bin/nterm");
  */
}

Context *schedule(Context *prev) {
  //Log("schedule called");
  // save the context pointer
  current->cp = prev;

  // always select pcb[0] as the new process
  current = &pcb[0];
  //current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);

  // then return the new context
  return current->cp;
}
