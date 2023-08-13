cmake_minimum_required(VERSION 3.12)

project(shared_memory)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# 添加可执行文件
add_executable(producer src/producer.cc)
add_executable(consumer src/consumer.cc)

# 设置编译选项（可以根据需要添加更多选项）
target_compile_options(producer PRIVATE 
    -Wall
    -Wextra
    -Werror
)
target_compile_options(consumer PRIVATE 
    -Wall
    -Wextra
    -Werror
)

# 设置输出路径（如果需要的话）
# set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)