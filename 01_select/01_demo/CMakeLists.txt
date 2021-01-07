CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

PROJECT(Select)

SET(CMAKE_CXX_STANDARD 11)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic" )

ADD_EXECUTABLE(select
    main.cc
    server.cc
    select.cc
)
