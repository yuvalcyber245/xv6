#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

#ifndef LAB_PGTBL
#define LAB_PGTBL 3
#endif
#ifdef LAB_PGTBL
#define MAX_PAGES_TO_SCAN 4096
#define BITS_IN_UINT32 32
// ! side note for anyone reading this. never write:
// ! #define BITS_IN_UINT32 sizeof(uint32)
// ! finding this bug might cost you way too much time
int
sys_pgaccess(void)
{
  pagetable_t pagetable = myproc()->pagetable;
  pte_t* pte = 0;
  uint64 va_start = 0;
  int size  = -1;
  uint64 return_addr = 0;
  // get arguments
  argaddr(0, &va_start);
  argint(1, &size);
  argaddr(2, &return_addr);
  uint32 bitmask[MAX_PAGES_TO_SCAN / BITS_IN_UINT32];
  // clear bitmask array
  for (int i = 0; i < size / BITS_IN_UINT32; i++) {
    bitmask[i] = 0;
  }
  if (size >= MAX_PAGES_TO_SCAN){
    size = MAX_PAGES_TO_SCAN;
  }
  for (int i = 0; i < size; i++) {
    pte = walk(pagetable, va_start + i * PGSIZE, 0 );
    if (pte == 0) {
      // va was not found, an error has ocurred
      return -1;
    }
    else {
      // put bit in bitmask and clear access bit
      bitmask[(i / BITS_IN_UINT32)]  |= (((PTE_A & *pte) / PTE_A) << (i % BITS_IN_UINT32));
      *pte ^= (*pte & PTE_A); // clear access bit
    }
  }
  // copy to user memory
  copyout(pagetable, return_addr, (char *)bitmask, size / sizeof(char));  
  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
