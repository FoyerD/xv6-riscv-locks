#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


int main(void){
    int tid;
    int result;
    int N = 16;

    
    printf("Creating tournament with %d processes\n", N);
    tid = tournament_create(N);
    if (tid < 0) {
        printf("7\n");
        exit(1);
    }
    
    result = tournament_acquire();
    if (result < 0) {
        exit(1);
    }
    // Critical section
    printf("Hello im proccess %d you forked me %d prepere to release\n", getpid(), tid);
    // End of critical section
    result = tournament_release();
    if (result < 0) {
        exit(1);
    }

    exit(0);
}