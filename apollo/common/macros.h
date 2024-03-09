#include <iostream>

namespace apollo {
namespace common {

template<typename T>
T* CheckNotNull(T* ptr, const char* ptr_name, const char* file, int line) {
  if (ptr == nullptr) {
    std::cerr << "Check failed: " << ptr_name << " is nullptr at "
              << file << ":" << line << std::endl;
    std::abort();
  }
  return ptr;
}

#define CHECK_NOTNULL(ptr) CheckNotNull((ptr), #ptr, __FILE__, __LINE__)

}  // namespace common
}  // namespace apollo