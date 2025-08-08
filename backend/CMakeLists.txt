cmake_minimum_required(VERSION 3.16)
project(itacati_server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_executable(itacati_server
    src/main.cpp
    src/api/HttpServer.cpp
    src/core/CallManager.cpp
    src/sip/SipStackDummy.hpp
)

# 后续：
# target_link_libraries(itacati_server PRIVATE pjsip pjmedia pjnath srtp webrtc ...) 

if (MSVC)
  target_compile_options(itacati_server PRIVATE /W4)
else()
  target_compile_options(itacati_server PRIVATE -Wall -Wextra -Wpedantic)
endif()