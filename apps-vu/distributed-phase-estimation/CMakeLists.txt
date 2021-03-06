cmake_minimum_required(VERSION 3.0.0)
project(distributed_phase_estimation VERSION 1.0.0)
option(arch "amd64/armhf" "amd64")
option(BUILD_TESTS  "Build tests" ON)
set(CMAKE_SYSTEM_NAME Linux)

set(riaps_prefix "/opt/riaps/" CACHE STRING "the riaps prefix")

#Set the platform
if (${arch} STREQUAL "armhf")
 set(TOOLCHAIN_PREFIX arm-linux-gnueabihf)
 set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
 set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
 set(CMAKE_FIND_ROOT_PATH /usr/${TOOLCHAIN_PREFIX})
 set (CMAKE_CXX_FLAGS "-std=c++11 -g")
 set (CMAKE_C_FLAGS "-std=c99 -g")
else()
 set(CMAKE_C_COMPILER gcc-5)
 set(CMAKE_CXX_COMPILER g++-5)
 set (CMAKE_CXX_FLAGS "-std=c++11 -D_GLIBCXX_USE_CXX11_ABI=0")
 set (CMAKE_C_FLAGS "-std=c99")
endif()

set(CMAKE_INSTALL_PREFIX ${riaps_prefix}/${arch})

set(DEPENDENCIES ${riaps_prefix})
set (LIBALLPATH_INCLUDE ${DEPENDENCIES}/${arch}/include)
set (LIBALLPATH_LIB ${DEPENDENCIES}/${arch}/lib)
include_directories(${LIBALLPATH_INCLUDE})
link_directories(${LIBALLPATH_LIB})


# Debug binaries are to be copied into "./bin" directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_HOME_DIRECTORY}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_HOME_DIRECTORY}/bin)

#add includes
include_directories(include)

add_library(generator SHARED src/Generator.cc
                             src/base/GeneratorBase.cc
                             include/messages/timertest.capnp.c++)

target_link_libraries(generator czmq riaps dl capnp kj soc)

add_library(receiver SHARED src/Receiver.cc
                            src/base/ReceiverBase.cc
                            include/messages/timertest.capnp.c++)

target_link_libraries(receiver czmq riaps dl capnp kj soc)

