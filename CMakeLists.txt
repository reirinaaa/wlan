cmake_minimum_required(VERSION 4.1.0)

project(wlan)

add_executable(wlan wlan.cpp)

target_link_libraries(wlan kernel32 user32 gdi32 wlanapi)
