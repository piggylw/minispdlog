# 收集所有源文件
set(MINISPDLOG_SOURCES
    level.cpp
    details/utils.cpp
    formatter.cpp
    patternformatter.cpp
    logger.cpp
    registry.cpp
    asynclogger.cpp
    details/threadpool.cpp
)

# 创建静态库
add_library(minispdlog STATIC ${MINISPDLOG_SOURCES})

# 包含目录
target_include_directories(minispdlog PUBLIC
    ${PROJECT_SOURCE_DIR}/include    
)

# 链接 fmt 库
target_link_libraries(minispdlog PUBLIC fmt::fmt)

# 设置编译特性
target_compile_features(minispdlog PUBLIC cxx_std_17)