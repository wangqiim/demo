#include <iostream>
#include <omp.h>

// http://zhaoxuhui.top/blog/2017/12/02/OpenMP%E5%B9%B6%E8%A1%8C%E7%BC%96%E7%A8%8B%E7%AC%94%E8%AE%B0.html
// https://zhuanlan.zhihu.com/p/497494198
char datas[11];

int main() {
#pragma omp parallel for  
	for (int i = 0; i < 6; i++)
		printf("i = %d, I am Thread %d\n", i, omp_get_thread_num());
	
	//这里是两个for循环之间的代码，将会由线程0即主线程执行
	printf("I am Thread %d\n", omp_get_thread_num());
	{
		char *ptr = (char *)datas;
		char *ptr_end = ptr + 10;
#pragma omp parallel for  
		for (ptr = (char *)datas; ptr < ptr_end; ptr++)
			*ptr = 'a';
	}

	{
		char *ptr = (char *)datas;
		char *ptr_end = ptr + 10;
#pragma omp parallel for  
		for (ptr = (char *)datas; ptr < ptr_end; ptr++) {
			printf("*ptr = %c, I am Thread %d\n", *ptr, omp_get_thread_num());
			printf("*ptr = %c, I am Thread %d\n", *ptr, omp_get_thread_num());
		}
	}
}
