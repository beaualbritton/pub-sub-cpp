cmake_minimum_required(VERSION 4.1)
project(pub-sub CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost REQUIRED )
find_package(Threads REQUIRED)

add_executable(server
    server/server.cpp
    server/session.cpp
)
add_executable(broker
    broker/broker.cpp
)

foreach(target broker server)

    target_link_libraries(${target} 
    PRIVATE Boost::headers
    Threads::Threads)

endforeach()
