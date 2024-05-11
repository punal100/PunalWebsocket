project(PunalWebScketBoostBeast)

cmake_minimum_required(VERSION 3.0.0)

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
#set(OPENLSSL_ROOT ${PROJECT_SOURCE_DIR}/openssl) # either set it here or from the command line
#include_directories(${OPENLSSL_ROOT}/include)
#message(STATUS "Openssl Directory Value is - ${OPENLSSL_ROOT}/include")
#set(OPENSSL_ROOT_DIR ${PROJECT_SOURCE_DIR}/openssl)
#find_package(OpenSSL REQUIRED)

#Example Files ONLY FOR TESTING
#include_directories(${BOOST_ROOT}/libs/beast/example/common)
add_executable(simple_example main.cpp)