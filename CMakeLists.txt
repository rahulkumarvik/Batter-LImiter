cmake_minimum_required(VERSION 3.16)
project(battery-limiter VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(battery-limiter
    src/main.cpp
    src/monitor.cpp
    src/logic.cpp
    src/event.cpp
    src/sound.cpp
    src/notify.cpp
)

target_include_directories(battery-limiter PRIVATE include)

install(TARGETS battery-limiter RUNTIME DESTINATION bin)
install(FILES config/default.json DESTINATION share/battery-limiter/config)
install(FILES systemd/battery-limiter.service DESTINATION lib/systemd/system)
