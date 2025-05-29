cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0148 NEW)
project(twitter_dm_static)

set(CMAKE_CXX_STANDARD 17)
# 添加cpr依赖
set(ENV{http_proxy} "http://127.0.0.1:7890")
set(ENV{https_proxy} "http://127.0.0.1:7890")
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
        GIT_TAG 1.10.5
)
FetchContent_MakeAvailable(cpr)


add_subdirectory(extern/pybind11)
set(LIB_NAME twitter_dm)

pybind11_add_module(${LIB_NAME} bindings.cpp twitter_dm.cpp)
target_link_libraries(${LIB_NAME} PRIVATE cpr::cpr nlohmann_json::nlohmann_json spdlog::spdlog)

install(TARGETS ${LIB_NAME} DESTINATION .)