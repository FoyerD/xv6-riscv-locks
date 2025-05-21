#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int L = 0;
int N = 0;
int num_locks = 0;
int* procs;
int* locks;




int
role_l(int i, int l)
{
    return (i & (1 << (L-l-1))) >> (L-l-1);
}

int
lock_l(int i, int l)
{
    return i >> (L-l);
}

int
lock_i(int i, int l)
{
    return lock_l(i, l) + (1 << l) - 1;
}

int
pow_2(int exp)
{
    int result = 1;
    for (int i = 0; i < exp; i++) {
        result <<= 1;
    }
    return result;
}



int
tournament_create(int processes)
{
    int log_counter = processes;
    int p = 0;
    int tournament_id = 0;

    N = processes;
    if ((N & (N - 1)) != 0) {
        printf("1\n");
        return -1;
    }

    while (log_counter > 1) {
        log_counter = log_counter / 2;
        L++;
    }

    num_locks = (pow_2(L) - 1);

    procs = malloc(N * sizeof(int));
    if (!procs) {
        printf("2\n");
        return -1;
    }
    

    locks = malloc(num_locks * sizeof(int));
    if (!locks) {
        free(procs);
        printf("3\n");
        return -1;
    }

    for (int i = 0; i < N; i++) {
        procs[i] = -1;
    }
    for (int i = 0; i < num_locks; i++) {
        locks[i] = peterson_create();
        if (locks[i] < 0) {
            printf("4\n");
            return -1;
        }
    }

    procs[0] = getpid();  // parent is index 0

    for (tournament_id = N - 1; tournament_id > 0; tournament_id--) {
        p = fork();
        if (p < 0) {
            printf("5\n");
            return -1;
        }
        if (p == 0) {
            procs[tournament_id] = getpid();
            break;
        }
    }
    return tournament_id;
}


int
tournament_acquire(void)
{
    int i = 0;
    int l = 0;
    int lock_id = 0;
    int ret_val = 0;

    for (i = 0; i < N; i++) {
        if (procs[i] == getpid()) {
            break;
        }
    }

    if (i == N) {
        printf("6-%d\n", getpid);
        return -1;
    }

    for (l = L-1; l >= 0; --l) {
        lock_id = lock_i(i, l);
        ret_val = peterson_acquire(locks[lock_id], role_l(i, l));
    }

    return ret_val;
}

int
tournament_release(void)
{
    int i = 0;
    int l = 0;
    int lock_id = 0;
    int ret_val = 0;

    for (i = 0; i < N; i++) {
        if (procs[i] == getpid()) {
            break;
        }
    }

    if (i == N) {
        return -1;
    }

    for (l = 0; l < L; l++) {
        lock_id = lock_i(i, l);
        ret_val = peterson_release(locks[lock_id], role_l(i, l));
    }

    return ret_val;
}