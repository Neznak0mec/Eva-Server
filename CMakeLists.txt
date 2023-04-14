cmake_minimum_required(VERSION 3.25)
project(Server)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(VOSK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/vosk)
link_directories(${VOSK_PATH})

find_package(asio CONFIG REQUIRED)
find_package(portaudio CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_executable(Server main.cpp)
target_link_libraries(Server portaudio vosk asio::asio nlohmann_json::nlohmann_json)