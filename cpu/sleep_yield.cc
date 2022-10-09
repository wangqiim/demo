#include <math.h>
#include <vector>
#include <unistd.h>
#include <thread>
#include <iostream>
#include <chrono>

const int maxn = 1e7;
const int yield_interval = 1e7;

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

void Bar() {
    using namespace std::chrono_literals;
    auto start = std::chrono::high_resolution_clock::now();
    int ans = 0;
    for (int i = 0; i < maxn * 10; i++) {
        ans += calcu(i);
        if (i % yield_interval == 0) {
            ans++;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end-start;
    std::cout << "Bar done, ans = " << ans << "Waited " << elapsed.count() << " ms\n";
}

void Foo() {
    using namespace std::chrono_literals;
    auto start = std::chrono::high_resolution_clock::now();
    int ans = 0;
    for (int i = 0; i < maxn; i++) {
        ans += calcu(i);
        if (i % yield_interval == 0) {
            ans++;
            sleep(0);
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end-start;
    std::cout << "Foo done, ans = " << ans << "Waited " << elapsed.count() << " ms\n";
}

int main() {
    unsigned int n = std::thread::hardware_concurrency();
    std::cout << "Bar thread = " << n << std::endl;
    std::cout << "Foo thread = 1" << std::endl;
    std::vector<std::thread> thread_group;
    thread_group.push_back(std::thread(Foo));
    for (int i = 0; i < n; i++) {
        thread_group.push_back(std::thread(Bar));
    }
    for (uint64_t thread_itr = 0; thread_itr < thread_group.size(); ++thread_itr) {
        thread_group[thread_itr].join();
    }
    std::cout << "main finish" << std::endl;
}