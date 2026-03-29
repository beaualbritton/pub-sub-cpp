cmake_minimum_required(VERSION 4.1)
project(pub-sub CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost REQUIRED COMPONENTS json)
find_package(Threads REQUIRED)

add_executable(server
    server/ws_server.cpp
    broker/broker.cpp
    lib/session.cpp
    lib/server.cpp
)
add_executable(broker
    broker/main.cpp
    broker/broker_server.cpp
    lib/session.cpp
    lib/server.cpp
)
foreach(target server broker)

    target_link_libraries(${target} 
    PRIVATE Boost::headers
    Boost::json
    Threads::Threads)

endforeach()
