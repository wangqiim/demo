#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "def.h"

int main() {
    // Open the shared memory
    int shm_fd = shm_open(SHARED_MEMORY_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }
    // ftruncate(shm_fd, sizeof(int)); by producer

    // Map the shared memory into process address space
    int* shared_data = static_cast<int*>(mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
    if (shared_data == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // Open the named condition variable
    int condvar_fd = shm_open(COND_VAR_NAME, O_CREAT | O_RDWR, 0666);
    if (condvar_fd == -1) {
        perror("shm_open (condvar)");
        exit(1);
    }
    // ftruncate(condvar_fd, sizeof(pthread_cond_t));  by producer

    // Map the named condition variable into process address space
    pthread_cond_t* condvar = static_cast<pthread_cond_t*>(mmap(NULL, sizeof(pthread_cond_t), PROT_READ | PROT_WRITE, MAP_SHARED, condvar_fd, 0));
    if (condvar == MAP_FAILED) {
        perror("mmap (condvar)");
        exit(1);
    }
    
    // Open the named mutex
    int mutex_fd = shm_open(MUTEX_NAME, O_RDWR, 0666);
    if (mutex_fd == -1) {
        perror("shm_open (mutex)");
        exit(1);
    }
    // ftruncate(mutex_fd, sizeof(pthread_mutex_t)); by producer

    // Map the named mutex into process address space
    pthread_mutex_t* mutex = static_cast<pthread_mutex_t*>(mmap(NULL, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, mutex_fd, 0));
    if (mutex == MAP_FAILED) {
        perror("mmap (mutex)");
        exit(1);
    }

    // consume data
    while (true) {
        pthread_mutex_lock(mutex);

        std::cout << *shared_data << std::endl;

        pthread_cond_wait(condvar, mutex);
        pthread_mutex_unlock(mutex);
    }

    // Clean up
    munmap(condvar, sizeof(pthread_cond_t));
    close(condvar_fd);
    munmap(mutex, sizeof(pthread_mutex_t));
    close(mutex_fd);
    munmap(shared_data, sizeof(int));
    close(shm_fd);

    return 0;
}
