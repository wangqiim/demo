#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>

// ref: https://www.jianshu.com/p/57fba10c2921
void test1(int fd) {
    char buf[8192] __attribute__((aligned(8192)));
    size_t count = 4096;
    ssize_t ret;

    while (count > 0) {
        ret = write(fd, buf, count);
        if (ret < 0) {
            printf("write %ld bytes failed. ret:%ld, %s\n",
                    count, ret, strerror(errno));
            break;
        }
        printf("write %ld bytes success. ret:%ld\n", count, ret);

        count /= 2;
    }

    return;
}

void test2(int fd) {
    char buf[8192] __attribute__((aligned(8192)));
    size_t count = 4096;
    ssize_t ret;

    while (count > 0) {
        ret = write(fd, buf + count, 512);
        if (ret < 0) {
            printf("write buf offset %ld failed. ret:%ld, %s\n",
                    count, ret, strerror(errno));
            break;
        }
        printf("write buf offset %ld failed. ret:%ld\n", count, ret);

        count /= 2;
    }

    return;
}

void test3(int fd) {
    char buf[8192] __attribute__((aligned(8192)));
    size_t count = 4096;
    ssize_t ret;

    while (count > 0) {
        int lseek_ret = lseek(fd, count, SEEK_SET);
        assert(lseek_ret == count);
        ret = write(fd, buf, 512);
        if (ret < 0) {
            printf("write fd offset %ld failed. ret:%ld, %s\n",
                    count, ret, strerror(errno));
            break;
        }
        printf("write fd offset %ld failed. ret:%ld\n", count, ret);

        count /= 2;
    }

    return;
}

int main()
{
    int fd = open("testfile", O_RDWR | O_CREAT | O_DIRECT, S_IRWXU | S_IRWXG | S_IRWXO);
    if (fd < 0) {
        printf("open file failed.\n");
        return -1;
    }
    int lseek_ret = -1;
    printf("1. test count.\n");
    lseek_ret = lseek(fd, 0, SEEK_SET);
    assert(lseek_ret == 0);
    test1(fd);

    printf("2. test buffer offset.\n");
    lseek_ret = lseek(fd, 0, SEEK_SET);
    assert(lseek_ret == 0);
    test2(fd);


    printf("3. test fd offset.\n");
    test3(fd);
    close(fd);

    return 0;
}
