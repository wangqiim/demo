#include <xmmintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <iostream>
#include <chrono>
#include <thread>

// https://blog.csdn.net/kongfuxionghao/article/details/47028919
// https://stackoverflow.com/questions/30207256/how-to-get-the-size-of-the-cpu-cache-in-linux

// $ g++ -O3 cpu_cache.cc -DEMPTYCALC

// 创建1G大小的文件用于mmap
// $ dd if=/dev/urandom of=random.img count=512 bs=1M

// ---------------案例1: l1/l2/l3 cache ------------------
// $ for x in 1 2 4 8 16 32 64 128 256 512 1024; do ./a.out random.img 1024 $x; done
// 可以大概获得cpu的cacheline的大小，结果与 lscpu | grep cache 查询的一致

void usage() {
    printf("usage: BIN file step prefetch\n");
    exit(1);
}

inline int calcu(int input) {
#ifdef EMPTYCALC
    return input;
#endif
    int val = (input % 99) * (input / 98);
    val = val ? val : 1;
#ifdef HEAVYCALC
    double d = (double)input / (double)val;
    return (int)pow(d, 1999.9);
#endif
    double n = sqrt(sqrt((double)(unsigned)input * 1.3));
    double m = sqrt(sqrt((double)(unsigned)val * 0.9));
    return (int)((double)input * (double)val * m / (n ? n : 1.1));
}

int run(const int *array, int size, int step, int blocksize) {
    int result = 0;
    blocksize *= 4096;
    if (blocksize == 0) {
        blocksize = size;
    }
    printf("run... (block=%d pages)\n", blocksize/4096);
    int start = 0;
    int nsize = blocksize;
    while (nsize == blocksize) {
        if (start + blocksize > size)
            nsize = size - start;
        for (int i = 0; i < step; i++) {
            for (int j = i; j < nsize; j += step) {
                result += calcu(array[start+j]);
            }
        }
        start += nsize;
    }
    return result;
}

int main(int argc, const char *argv[]) {
    if (argc != 4) {
        usage();
    }

    int step = atoi(argv[2]);
    int blocksize = atoi(argv[3]);
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        usage();
    }
    struct stat st;
    int ret = fstat(fd, &st);
    if (ret != 0) {
        usage();
    }

    int array_size = st.st_size / sizeof(int);
    printf("array size: %d, step: %d. ", array_size, step);

    const int *array = (const int *)mmap(NULL, st.st_size, PROT_READ, MAP_POPULATE|MAP_SHARED, fd, 0);
    if (array == MAP_FAILED) {
        usage();
    }

    using namespace std::chrono_literals;
    auto start = std::chrono::high_resolution_clock::now();
    int result = 0;
    result = run(array, array_size, step, blocksize);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end-start;
    std::cout << "Elapsed " << elapsed.count() << " ms\n";

    return result;
}