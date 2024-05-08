cmake_minimum_required (VERSION 3.0)
project (musicophile)

if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please select another Build Directory" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
	message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
	message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()

# Include SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Include irrKlang
include_directories(
    external/irrKlang_1_6_0/include/
	.
)

link_directories(
    external/irrKlang_1_6_0/lib/
    /usr/local/mysql/lib  # Add MySQL library directory
)    

set(ALL_LIBS
    libIrrKlang.so
    mysqlcppconn
)

# Set C++ standard and flags
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -g")
set(CMAKE_BUILD_TYPE Debug)

# Define executable
add_executable(musicClient
    src/network.cpp
	src/client/main.cpp   
    src/client/audio.cpp
    src/client/client.cpp
    # src/ui.cpp
)

# Link against libraries
target_link_libraries(musicClient
    ${ALL_LIBS}
    sfml-graphics
    sfml-window
    sfml-system
)

# Copy executable to bin directory
add_custom_command(
   TARGET musicClient POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/musicClient${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/bin"  
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/external/irrKlang_1_6_0/lib/ikpMP3.dll" "${CMAKE_CURRENT_SOURCE_DIR}/bin" 
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/external/irrKlang_1_6_0/lib/irrKlang.dll" "${CMAKE_CURRENT_SOURCE_DIR}/bin" 
)

# Define musicServer executable (similarly for musicServer)
add_executable(musicServer
    src/network.cpp
	src/server/mainServer.cpp
    src/server/server.cpp
)
target_link_libraries(musicServer
    sfml-graphics
    sfml-window
    sfml-system
    ${IRRKLANG_LIBRARY}
)
add_custom_command(
   TARGET musicServer POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/musicServer${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/bin"  
)
