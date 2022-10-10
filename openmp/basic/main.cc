#include <iostream>
#include <omp.h>

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
