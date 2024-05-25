project(PunalWebScketBoostBeast)

cmake_minimum_required(VERSION 3.0.0)

include(ExternalProject)
add_compile_options(/bigobj)#In Case it Complains fatal error C1128: number of sections exceeded object file format limit: compile with /bigobj 
#set(simple_example)

#Macros Start
MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

MACRO(SUBFOLDEREXITS doesexist curdir foldertosearch)  
  set(${doesexist} 0)

  if(EXISTS ${curdir}/${foldertosearch})
    if(IS_DIRECTORY ${curdir}/${foldertosearch})
        set(${doesexist} 1)
    endif()
  endif()
ENDMACRO()
#Macros End

#Global Vars Start
SET(P_Should_Build_OpenSSL 0)
#Global Vars End

#set(CMAKE_CONFIGURATION_TYPES "Debug;Release;MinSizeRel;RelWithDebInfo" CACHE STRING "Supported configs" FORCE)

#set(Boost_DEBUG 1)
#set(BOOST_INCLUDEDIR ${PROJECT_SOURCE_DIR}/boost)
#set(BOOST_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/boost)
set(BOOST_ROOT ${PROJECT_SOURCE_DIR}/boost/boost) # either set it here or from the command line

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF)
#set(BOOST_INCLUDE_LIBRARIES thread filesystem system program_options asio date_timec beast)
#set(BOOST_ENABLE_CMAKE ON)

#Adding All Boost Sud Directories
SUBDIRLIST(P_BOOST_SUB_MODULES ${BOOST_ROOT}/libs)
FOREACH(subdir ${P_BOOST_SUB_MODULES})  
    set(P_Local_SubDir_Exists False)
    SUBFOLDEREXITS(P_Local_SubDir_Exists ${BOOST_ROOT}/libs/${subdir} "include")

    #message(STATUS "P_Local_SubDir_Exists Value is - ${P_Local_SubDir_Exists}")

    if (P_Local_SubDir_Exists EQUAL 1)
        include_directories(${BOOST_ROOT}/libs/${subdir}/include)
    else()
        SUBDIRLIST(P_Local_Boost_SubDirectories ${BOOST_ROOT}/libs/${subdir})
        
        FOREACH(Local_subdir ${P_Local_Boost_SubDirectories})  
            set(P_Local_SubDir_Exists 0)
            SUBFOLDEREXITS(P_Local_SubDir_Exists ${BOOST_ROOT}/libs/${subdir}/${Local_subdir} include)
            if (P_Local_SubDir_Exists EQUAL 1)
                include_directories(${BOOST_ROOT}/libs/${subdir}/${Local_subdir}/include)
            endif()
        ENDFOREACH()
    endif()

ENDFOREACH()

#Adding OpenSSL
set(OPENLSSL_ROOT ${PROJECT_SOURCE_DIR}/openssl) # either set it here or from the command line
include_directories(${OPENLSSL_ROOT}/include)
message(STATUS "Openssl Directory Value is - ${OPENLSSL_ROOT}/include")
set(OPENSSL_ROOT_DIR ${PROJECT_SOURCE_DIR}/openssl)
#find_package(OpenSSL REQUIRED)

#set(OPENSSL_SOURCE_DIR ${PROJECT_SOURCE_DIR}/openssl) # default path by CMake
#set(OPENSSL_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/openssl)
#set(OPENSSL_INCLUDE_DIR ${OPENSSL_INSTALL_DIR}/include)
#set(OPENSSL_CONFIGURE_COMMAND ${OPENSSL_SOURCE_DIR}/Configure)
#ExternalProject_Add(
#  OpenSSL
#  SOURCE_DIR ${OPENSSL_SOURCE_DIR}
#  USES_TERMINAL_DOWNLOAD TRUE
#  CONFIGURE_COMMAND
#    perl ${OPENSSL_CONFIGURE_COMMAND}
#    --prefix=${OPENSSL_INSTALL_DIR}
#    --openssldir=${OPENSSL_INSTALL_DIR}
#  BUILD_COMMAND make
#  TEST_COMMAND ""
#  INSTALL_COMMAND make install
#  INSTALL_DIR ${OPENSSL_INSTALL_DIR}
#)
#
## We cannot use find_library because ExternalProject_Add() is performed at build time.
## And to please the property INTERFACE_INCLUDE_DIRECTORIES,
## we make the include directory in advance.
#file(MAKE_DIRECTORY ${OPENSSL_INCLUDE_DIR})
#
#add_library(OpenSSL::SSL STATIC IMPORTED GLOBAL)
#set_property(TARGET OpenSSL::SSL PROPERTY IMPORTED_LOCATION ${OPENSSL_INSTALL_DIR}/lib/libssl.${OPENSSL_LIBRARY_SUFFIX})
#set_property(TARGET OpenSSL::SSL PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${OPENSSL_INCLUDE_DIR})
#add_dependencies(OpenSSL::SSL OpenSSL)
#
#add_library(OpenSSL::Crypto STATIC IMPORTED GLOBAL)
#set_property(TARGET OpenSSL::Crypto PROPERTY IMPORTED_LOCATION ${OPENSSL_INSTALL_DIR}/lib/libcrypto.${OPENSSL_LIBRARY_SUFFIX})
#set_property(TARGET OpenSSL::Crypto PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${OPENSSL_INCLUDE_DIR})
#add_dependencies(OpenSSL::Crypto OpenSSL)

#OpenSSL Building
if(CMAKE_HOST_WIN32)   
    set(P_Visual_Studio_Pre_Build_Command_Target)
    if(NOT CMAKE_CL_64)
        set(P_OPENSSL_BUILD_COMMAND_1 perl ${OPENSSL_ROOT_DIR}/Configure VC-WIN32)
    else()
        set(P_OPENSSL_BUILD_COMMAND_1 perl ${OPENSSL_ROOT_DIR}/Configure VC-WIN64A)
    endif()
    set(P_OPENSSL_BUILD_COMMAND_2 nmake clean)
    set(P_OPENSSL_BUILD_COMMAND_3 nmake)
endif()
if(CMAKE_HOST_APPLE)
  set(P_OPENSSL_BUILD_COMMAND_1 perl ${OPENSSL_ROOT_DIR}/Configure)
  set(P_OPENSSL_BUILD_COMMAND_2 make clean)
  set(P_OPENSSL_BUILD_COMMAND_3 make)
endif()

IF(P_Should_Build_OpenSSL)
    set(P_OPENSSL_BUILD_COMMAND_VAR)
    add_custom_target(
        P_OPENSSL_BUILD_COMMAND_VAR ALL
        WORKING_DIRECTORY ${OPENSSL_ROOT_DIR}
        COMMAND ${P_OPENSSL_BUILD_COMMAND_1}
        COMMAND ${P_OPENSSL_BUILD_COMMAND_2}
        COMMAND ${P_OPENSSL_BUILD_COMMAND_3}
        BYPRODUCTS ${OPENSSL_ROOT_DIR}/libcrypto.lib ${OPENSSL_ROOT_DIR}/libssl.lib
        USES_TERMINAL
    )
ENDIF()

if(WIN32)
    if (CMAKE_GENERATOR MATCHES "Visual Studio")            
        set(P_Visual_Studio_Pre_Build_Command_Target)
        if(NOT CMAKE_CL_64)
            set(P_Visual_Studio_Pre_Build_Command "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvarsall.bat" x86)
        else()
            set(P_Visual_Studio_Pre_Build_Command "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvarsall.bat" x64)
        endif()

        #add_custom_target(
        #        P_Visual_Studio_Pre_Build_Command_Target ALL
        #        COMMAND ${P_Visual_Studio_Pre_Build_Command}
        #        USES_TERMINAL
        #    )
    endif()
endif()


#ExternalProject_Add(
#  OpenSSL
#  SOURCE_DIR ${OPENSSL_ROOT_DIR}
#  BUILD_IN_SOURCE True
#  CONFIGURE_COMMAND
#    ${P_OPENSSL_BUILD_COMMAND_1}
#  BUILD_COMMAND make
#  BUILD_BYPRODUCTS ${OPENSSL_ROOT_DIR}/libcrypto.lib ${OPENSSL_ROOT_DIR}/libssl.lib
#)

#Example Files ONLY FOR TESTING
include_directories(${BOOST_ROOT}/libs/beast/example)
add_executable(simple_example main.cpp)
IF(P_Should_Build_OpenSSL)
    add_dependencies(simple_example P_OPENSSL_BUILD_COMMAND_VAR)
ENDIF()
if(WIN32)
    if (CMAKE_GENERATOR MATCHES "Visual Studio")
    #add_dependencies(P_OPENSSL_BUILD_COMMAND_VAR P_Visual_Studio_Pre_Build_Command_Target)
    endif()
endif()

#OpenSSL Library Linking
if(WIN32)
  target_link_libraries(simple_example ${OPENSSL_ROOT_DIR}/libcrypto.lib)
  target_link_libraries(simple_example ${OPENSSL_ROOT_DIR}/libssl.lib)
endif()
if(APPLE)
  target_link_libraries(simple_example ${OPENSSL_ROOT_DIR}/libcrypto.a)
  target_link_libraries(simple_example ${OPENSSL_ROOT_DIR}/libssl.a)
endif()