cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0148 NEW)
project(twitter_dm)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# 检查是否需要为macOS ARM架构构建
if(APPLE)
  # 检查是否设置了TARGET_ARCH环境变量
  if(DEFINED ENV{TARGET_ARCH})
    if("$ENV{TARGET_ARCH}" STREQUAL "arm64")
      set(CMAKE_OSX_ARCHITECTURES "arm64")
      message(STATUS "为macOS ARM架构(Apple Silicon)构建")
    elseif("$ENV{TARGET_ARCH}" STREQUAL "x86_64")
      set(CMAKE_OSX_ARCHITECTURES "x86_64")
      message(STATUS "为macOS x86_64架构(Intel)构建")
    endif()
  endif()
endif()
# 添加一个选项来控制 Python 环境
option(USE_LOCAL_PYTHON_ENV "使用本地 Python 环境" ON)

if(USE_LOCAL_PYTHON_ENV)
    message(STATUS "使用本地 Python 环境")
    # 在这里设置您的本地 Python 路径
    # 例如：set(Python_EXECUTABLE /usr/local/bin/python3)
    # find_package(Python COMPONENTS Interpreter Development REQUIRED)
    # message(STATUS "Python_EXECUTABLE: ${Python_EXECUTABLE}")
    # message(STATUS "Python_INCLUDE_DIRS: ${Python_INCLUDE_DIRS}")
    # message(STATUS "Python_LIBRARIES: ${Python_LIBRARIES}")
else()
    message(STATUS "使用容器/特定路径 Python 环境")
    # 指定 Python 解释器路径
    set(Python_EXECUTABLE /root/miniconda3/envs/twitter_dm/bin/python3.12)
    # 指定 Python 包含目录
    set(Python_INCLUDE_DIRS /root/miniconda3/envs/twitter_dm/include/python3.12)
    # 指定 Python 库路径
    set(Python_LIBRARIES /root/miniconda3/envs/twitter_dm/lib/libpython3.12.so) # 或者可能是 .dylib，取决于您的系统和 Python 安装
endif()

set(CMAKE_CXX_STANDARD 17)

# 添加一个选项来控制是否设置代理环境变量
option(ENABLE_PROXY_ENV_SETTINGS "启用 FetchContent 的 http_proxy 和 https_proxy 环境变量设置" OFF)

if(ENABLE_PROXY_ENV_SETTINGS)
    message(STATUS "已为 FetchContent 启用代理环境变量。")
    # 添加cpr依赖时设置代理
    set(ENV{http_proxy} "http://127.0.0.1:7890")
    set(ENV{https_proxy} "http://127.0.0.1:7890")
else()
    message(STATUS "已为 FetchContent 禁用代理环境变量。")
endif()

include(FetchContent)

# 添加 spdlog
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.12.0
)
FetchContent_MakeAvailable(spdlog)
# 添加 nlohmann_json
FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(
        cpr
        GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 1.11.2
)
FetchContent_MakeAvailable(cpr)

# 添加 pybind11 - 下载到 extern 目录
FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11.git
        GIT_TAG v2.11.1
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/pybind11
)
FetchContent_MakeAvailable(pybind11)

# 创建一个静态库，包含核心逻辑
add_library(twitter_dm_lib STATIC twitter_dm.cpp twitter_dm.h)
# 设置 -fPIC 选项，以便静态库可以被用于创建共享库
set_target_properties(twitter_dm_lib PROPERTIES POSITION_INDEPENDENT_CODE ON)
# 为静态库链接依赖
target_link_libraries(twitter_dm_lib PUBLIC cpr::cpr nlohmann_json::nlohmann_json spdlog::spdlog)

# 创建 Python 模块，并链接到静态库
pybind11_add_module(${PROJECT_NAME} MODULE bindings.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE twitter_dm_lib)

# 设置RPATH，使Python扩展模块能够找到依赖的动态库
if(APPLE)
    # 在macOS上，设置RPATH为@loader_path，这样扩展模块可以在同一目录中查找动态库
    set_target_properties(${PROJECT_NAME} PROPERTIES 
        INSTALL_RPATH "@loader_path"
        BUILD_WITH_INSTALL_RPATH TRUE
    )
elseif(UNIX AND NOT APPLE)
    # 在Linux上，设置RPATH为$ORIGIN，这样扩展模块可以在同一目录中查找动态库
    set_target_properties(${PROJECT_NAME} PROPERTIES 
        INSTALL_RPATH "$ORIGIN"
        BUILD_WITH_INSTALL_RPATH TRUE
    )
endif()

# 安装目标文件
install(TARGETS ${PROJECT_NAME} twitter_dm_lib DESTINATION .)

# 安装依赖的动态库
if(APPLE OR UNIX)
    # 使用生成器表达式获取CPR库的位置
    set(CPR_LIBRARY $<TARGET_FILE:cpr::cpr>)
    
    # 安装CPR库
    install(FILES ${CPR_LIBRARY} DESTINATION .)
    
    # 使用生成器表达式获取CURL库的位置
    set(CURL_LIBRARY $<TARGET_FILE:CURL::libcurl>)
    
    # 安装CURL库
    install(FILES ${CURL_LIBRARY} DESTINATION .)
    
    # 安装ZLIB库（如果需要）
    find_package(ZLIB)
    if(ZLIB_FOUND)
        # 使用ZLIB_LIBRARIES变量获取ZLIB库的路径
        foreach(LIB ${ZLIB_LIBRARIES})
            if(EXISTS "${LIB}")
                install(FILES "${LIB}" DESTINATION .)
            endif()
        endforeach()
    endif()
endif()

# 添加测试
if(BUILD_TESTING)
    # 添加头文件目录，确保测试子目录可以访问
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
    enable_testing()
    add_subdirectory(test)
endif()
