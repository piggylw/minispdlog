add_executable(testperformance testperformance.cpp)
target_link_libraries(testperformance PRIVATE minispdlog)


find_package(spdlog CONFIG REQUIRED)
# 创建benchmark可执行文件
add_executable(spdlog_benchmark testspdlog.cpp)

# 链接spdlog库
target_link_libraries(spdlog_benchmark PRIVATE spdlog::spdlog)

# 设置目标属性
target_compile_features(spdlog_benchmark PRIVATE cxx_std_17)

# 添加线程库（spdlog依赖）
find_package(Threads REQUIRED)
target_link_libraries(spdlog_benchmark PRIVATE Threads::Threads)