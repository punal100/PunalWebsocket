project(PunalWebScketBoostBeast)

cmake_minimum_required(VERSION 3.0.0)

#set(CMAKE_CONFIGURATION_TYPES "Debug;Release;MinSizeRel;RelWithDebInfo" CACHE STRING "Supported configs" FORCE)

#Include Directory Path Variables
set(BOOST_ASIO_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/asio/include)
set(BOOST_BEAST_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/beast/include)

#Including Include Directories
include_directories(${BOOST_ASIO_INCLUDE_DIR})
include_directories(${BOOST_BEAST_INCLUDE_DIR})

add_executable(simple_example main.cpp)