cmake_minimum_required(VERSION 3.10)
project(ChessGame)

# Set C++ standard  
set(CMAKE_CXX_STANDARD 17)  
set(CMAKE_CXX_STANDARD_REQUIRED True)  

# Include directories  
include_directories(include /opt/homebrew/include)

# Source files  
set(SOURCES  
    source/main.cc  
    source/game.cc  
    source/interface.cc  
)  

# Create the executable  
add_executable(Chess_game ${SOURCES})  

# Link SDL2 libraries  
target_link_libraries(Chess_game   
    /opt/homebrew/lib/libSDL2.dylib   
    /opt/homebrew/lib/libSDL2_image.dylib  
)  