// Mutual exclusion lock peterson edition.

#define NPLOCKS 16

struct petersonlock {
    uint turn;
    uint b[2];
    int initialized;      // Is the lock initialized?
};
  
int peterson_create(void);
int peterson_acquire(int lock_id, int role);
int peterson_release(int lock_id, int role);
int peterson_destroy(int lock_id);