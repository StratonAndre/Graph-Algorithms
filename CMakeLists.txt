cmake_minimum_required(VERSION 3.30)
project(AG_REVAMP)

set(CMAKE_CXX_STANDARD 20)

# Set SFML path
set(SFML_DIR "${CMAKE_SOURCE_DIR}/external/SFML-2.6.2/lib/cmake/SFML")
set(SFML_STATIC_LIBRARIES TRUE)

# Add tinyxml2 source first
set(TINYXML2_SOURCE
        ${CMAKE_SOURCE_DIR}/external/tinyxml2/tinyxml2.cpp
)

# Source files (including tinyxml2)
set(SOURCES
        src/main.cpp
        src/Animation.cpp
        src/Button.cpp
        src/Node.cpp
        src/UIPanel.cpp
        ${TINYXML2_SOURCE}  # Add tinyxml2 here
)

# Header files
set(HEADERS
        include/Animation.hpp
        include/BackgroundGrid.hpp
        include/Button.hpp
        include/Coordinates.hpp
        include/Edge.hpp
        include/Graph.hpp
        include/Node.hpp
        include/Theme.hpp
        include/UIPanel.hpp
        include/AlgorithmPanel.hpp
        include/MapParser.hpp
        include/algorithms/PathFinding.hpp
        include/GraphIO.hpp
        include/ViewportManager.hpp
        include/Application.hpp
        src/Application.cpp
        include/algorithms/MSTAlgorithm.hpp
        include/MSTPanel.hpp
        include/MSTTypes.hpp
        include/MSTVisualization.hpp
        include/algorithms/KruskalMST.hpp
        include/algorithms/BoruvkaMST.hpp
        include/algorithms/Ford-Fulkerson.hpp
        include/LuxembourgMapLoader.hpp  # Add this line
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Add include directories (both include and external)
target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/external
)

# Find and link SFML
find_package(SFML 2.6 COMPONENTS system window graphics network REQUIRED)

# Link SFML
target_link_libraries(${PROJECT_NAME}
        sfml-system
        sfml-window
        sfml-graphics
        sfml-network
)

# Ensure resources directory exists in build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/resources"
)

# Copy resources to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/resources"
        "${CMAKE_BINARY_DIR}/resources"
        COMMENT "Copying resources directory to build directory"
)