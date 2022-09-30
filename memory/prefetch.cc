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

// $ g++ -O3 prefetch.cc -DEMPTYCALC
// $ g++ -O3 prefetch.cc -DHEAVYCALC

// 创建1G大小的文件用于mmap
// $ dd if=/dev/urandom of=random.img count=512 bs=1M

// ---------------案例1: different step ------------------
// $ g++ -O3 prefetch.cc -DEMPTYCALC
// 无预取, step是1
// $ ./a.out random.img 1 0

// $ ./a.out random.img 64 0
// 分析: 此种情况下，由于locality,导致由于step的增加速度变慢了

// ---------------案例2: memory-bound cpu-not-bound ------------------
// $ g++ -O3 prefetch.cc -DEMPTYCALC
// 无预取, step是1
// $ ./a.out random.img 1 0

// 有预取, step是1
// $ ./a.out random.img 1 1
// 分析: 此种情况下，由于预取要引入更多的指令，速度变慢了

// ---------------案例3: memory-bound cpu-bound ------------------
// $ g++ -O3 prefetch.cc
// 无预取, step是1
// $ ./a.out random.img 1 0

// 有预取, step是1
// $ ./a.out random.img 1 1
// 分析: 此种情况下，由于预取指令的开销相对于计算，并不是很大。因此引入预取可以忽略不计了

// ---------------案例3: memory-bound cpu-bound step different------------------
// $ g++ -O3 prefetch.cc
// 无预取, step是64
// $ ./a.out random.img 64 0

// 有预取, step是64
// $ ./a.out random.img 64 1

// 有预取, step是64
// $ ./a.out random.img 64 2
// 分析: 此种情况下，由于64的步长会造成L1 cacheline miss，因此预取开始带来作用了。

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

int run_withprefetch(const int *array, int size, int step, int prefetch) {
    int result = 0;
    printf("run with prefetch(%d)...\n", prefetch);
    for (int i = 0; i < step; i++) {
        for (int j = i; j < size; j += step) {
            int k = j + step * prefetch;
            if (k  < size) {
                _mm_prefetch(&array[k], _MM_HINT_T0);
                //const int *addr = &array[k];
                //__asm__ __volatile__ ("mov (%0), %%eax"::"r"(addr):"eax");
                //__asm__ __volatile__ ("mov %0, %%eax"::"r"(k):"eax");
            }
            result += calcu(array[j]);
        }
    }
    return result;
}

int run(const int *array, int size, int step) {
    int result = 0;
    printf("run...\n");
    for (int i = 0; i < step; i++) {
        for (int j = i; j < size; j += step) {
            //asm volatile("lfence"); // 用于测试内存多通道
            result += calcu(array[j]);
        }
    }
    return result;
}

int main(int argc, const char *argv[]) {
    if (argc != 4) {
        usage();
    }

    int step = atoi(argv[2]);
    int prefetch = atoi(argv[3]);
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
    if (prefetch == 0) {
        result = run(array, array_size, step);
    } else if (prefetch > 0) {
        result = run_withprefetch(array, array_size, step, prefetch);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end-start;
    std::cout << "Elapsed " << elapsed.count() << " ms\n";

    return result;
}