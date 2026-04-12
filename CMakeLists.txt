cmake_minimum_required(VERSION 4.1)
project(pub-sub CXX)
include (FetchContent)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


find_package(Boost REQUIRED COMPONENTS json)

add_executable(server
    server/ws_server.cpp
    broker/broker_client.cpp
    lib/session.cpp
    lib/server.cpp
)
add_executable(broker
    broker/main.cpp
    broker/broker_server.cpp
    broker/logger.cpp
    lib/session.cpp
    lib/server.cpp
)
foreach(target server broker)

    target_link_libraries(
    ${target}
    PRIVATE Boost::headers
    Boost::json
    )

endforeach()

option(BROKER "build broker w/libpq" OFF)
if(BROKER)
  FetchContent_Declare(
    libpqxx
    GIT_REPOSITORY https://github.com/jtv/libpqxx.git
    GIT_TAG        8.0.1
  )
  FetchContent_MakeAvailable(libpqxx)
endif()

target_link_libraries(broker PRIVATE pqxx)
