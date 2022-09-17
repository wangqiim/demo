#include <iostream>
#include <thread>
#include <cstdio>
#include <mutex>

#pragma pack(1)
struct Obj {
    char padding[128];
}__attribute__ ((aligned(64)));

Obj obj;

std::mutex mtx_;
volatile uint16_t *a;
// uint16_t *a;

void func() {
    while (true) {
        // std::lock_guard<std::mutex> guard(mtx_);
        uint16_t tmp = *a;
        if (tmp != 0 && tmp != std::numeric_limits<uint16_t>::max()) {
            printf(" *a = %x\n", tmp);
            exit(1);
        }
        *a = ~(*a);
    }
}

int main() {
    a = (uint16_t *)(obj.padding + 63);
    // a = (uint16_t *)(obj.padding + 31);
    // a = (uint16_t *)obj.padding;
    std::cout << sizeof(Obj) << std::endl;
    // obj = new Obj();
    std::cout << &obj << std::endl;
    auto t1=std::thread(func);
    func();
    return 0;
}
