// Mutual exclusion peterson locks.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "petersonlock.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

struct petersonlock plocks[NPLOCKS];


void
peterson_init(void)
{
    for (int i = 0; i < NPLOCKS; i++) {
        plocks[i].initialized = 0;
        plocks[i].turn = 0;
        plocks[i].b[0] = 0;
        plocks[i].b[1] = 0;
    }
}

int
peterson_create(void)
{
    int plock_index = 0;
    int plock_count = 0;
    int i_to_return = -1;
    __sync_synchronize();

    while(__sync_lock_test_and_set(&plocks[plock_index].initialized, 1) == 1) {
        plock_index = (plock_index + 1) % NPLOCKS;
        plock_count++;
        if (plock_count >= NPLOCKS) {
            return -1; // No more locks available
        }
    }
    plocks[plock_index].turn = 0;
    plocks[plock_index].b[0] = 0;
    plocks[plock_index].b[1] = 0;

    i_to_return = plock_index;
    return i_to_return;
}

int
peterson_acquire(int lock_id, int role)
{
    __sync_synchronize();
    if(lock_id < 0 || 
       lock_id >= NPLOCKS || 
       !plocks[lock_id].initialized ||
       (role != 0 && role != 1)) {
        return -1; // Invalid lock ID
    }
    if(plocks[lock_id].b[role] == 1) {
        panic("pacquire"); // Lock already held by this role
    }
    __sync_lock_test_and_set(&plocks[lock_id].b[role], 1);
    __sync_lock_test_and_set(&plocks[lock_id].turn, role);
    __sync_synchronize();

    while(plocks[lock_id].b[1-role] && plocks[lock_id].turn == role) {
        yield();
    }
    __sync_synchronize();
    return 0;
}

int
peterson_release(int lock_id, int role)
{
    __sync_synchronize();
    if(lock_id < 0 || 
       lock_id >= NPLOCKS || 
       !plocks[lock_id].initialized ||
       (role != 0 && role != 1)) {
        return -1; // Invalid lock ID
    }
    if(plocks[lock_id].b[role] == 0) {
        panic("prelease"); // Lock not held by this role
    }
    __sync_synchronize();
    __sync_lock_release(&plocks[lock_id].b[role]);
    __sync_synchronize();
    return 0; 
}

int
peterson_destroy(int lock_id)
{
    __sync_synchronize();
    if(lock_id < 0 || 
       lock_id >= NPLOCKS || 
       !plocks[lock_id].initialized) {
        return -1; // Invalid lock ID
    }
    __sync_lock_release(&plocks[lock_id].initialized);
    return 0;
}