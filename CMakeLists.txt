# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required(VERSION 3.24)
project(UDPGameNetworking)

set(EXTERNAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external")

find_package(SDL3 REQUIRED CONFIG
    PATHS "${EXTERNAL_DIR}/SDL3"
    NO_DEFAULT_PATH
)
find_package(SDL3_net REQUIRED CONFIG
    PATHS "${EXTERNAL_DIR}/SDL3_net"
    NO_DEFAULT_PATH
)

set(LIBRARY_SOURCES
    UDPGameNetworking/NetworkUtilities.cpp
    UDPGameNetworking/ImportantMessage.cpp
    UDPGameNetworking/NetworkMessage.cpp
    UDPGameNetworking/Endpoints/Server.cpp
    UDPGameNetworking/Endpoints/Client.cpp
    UDPGameNetworking/Endpoints/SocketHolder.cpp
    UDPGameNetworking/NetworkObjects/NetworkObject.cpp
    UDPGameNetworking/NetworkObjects/OwnedNO.cpp
    UDPGameNetworking/NetworkObjects/UnownedNO.cpp
    UDPGameNetworking/NetworkObjects/ObjectDataProcessor.cpp
    UDPGameNetworking/CustomStreaming/PositionLerp2D.cpp
    UDPGameNetworking/CustomStreaming/NetworkedValue.cpp
    UDPGameNetworking/Senders/MessageSender.cpp
    UDPGameNetworking/Senders/ServerMessageSender.cpp
    UDPGameNetworking/Senders/ClientMessageSender.cpp
    UDPGameNetworking/Wrapper/Callback.cpp
)
set(DEMO_SOURCES
    UDPGameNetworking/UDPGameNetworking.cpp
    UDPGameNetworking/Testing/TestingFunctions.cpp
    UDPGameNetworking/Demo/Demo.cpp
    UDPGameNetworking/Demo/DemoWrapper.cpp
    UDPGameNetworking/Demo/DemoCallback.cpp
    UDPGameNetworking/Demo/DemoColourSquare.cpp
    UDPGameNetworking/Demo/ColourValue.cpp
)

# 1. Build a shared library from the core source files
add_library(UDPGameNetworking SHARED ${LIBRARY_SOURCES})

target_compile_definitions(UDPGameNetworking PRIVATE UDPGAMENETWORKING_EXPORTS)

# 2. Expose the public header files
target_include_directories(UDPGameNetworking PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/UDPGameNetworking>
)

# 3. Link the library against fetched SDL targets safely for EXPORT
target_link_libraries(UDPGameNetworking PUBLIC
    $<BUILD_INTERFACE:SDL3::SDL3-shared>
    $<BUILD_INTERFACE:SDL3_net::SDL3_net-shared>
)

# 4. Build a separate executable for the demo
add_executable(DemoApp ${DEMO_SOURCES})
add_dependencies(DemoApp UDPGameNetworking)

# 5. Link the demo executable to your new library
target_link_libraries(DemoApp PRIVATE UDPGameNetworking)

# 6. Copy dynamic DLL files to executable directory on Windows
if(WIN32)
    add_custom_command(TARGET DemoApp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3-shared>
        $<TARGET_FILE:SDL3_net::SDL3_net-shared>
        $<TARGET_FILE_DIR:DemoApp>
    )
endif()

# 7. Add installation rules to package the library
install(TARGETS UDPGameNetworking
    EXPORT UDPGameNetworkingTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Install the public header files
install(DIRECTORY UDPGameNetworking/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

# Config package export for find_package() support
install(EXPORT UDPGameNetworkingTargets
    FILE UDPGameNetworkingConfig.cmake
    NAMESPACE UDPGameNetworking::
    DESTINATION lib/cmake/UDPGameNetworking
)