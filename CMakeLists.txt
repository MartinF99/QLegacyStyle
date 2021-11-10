cmake_minimum_required(VERSION 3.0)
project(Q5LegacyStyle)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# Instruct CMake to create code from Qt designer ui files
set(CMAKE_AUTOUIC ON)

# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)

set(q5legacystyle_SRC
  src/main.cpp
  src/q5legacystyle.cpp
)

# Tell CMake to create the helloworld executable
add_executable(q5legacystyle ${q5legacystyle_SRC})

# Use the Widgets module from Qt 5.
target_link_libraries(q5legacystyle Qt5::Widgets)

# Install the executable
install(TARGETS q5legacystyle DESTINATION bin)
