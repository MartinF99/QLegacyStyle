cmake_minimum_required(VERSION 3.16)
project(QLegacyStyle)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# Instruct CMake to create code from Qt designer ui files
set(CMAKE_AUTOUIC ON)

# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)

set(qlegacystyle_SRC
  src/main.cpp
  src/qlegacystyle.h
  src/qlegacystyle.cpp
  src/widgetgallery.h
  src/widgetgallery.cpp
  src/drawutil.h
  src/drawutil.cpp
  src/bitmaps.h
  )

# Tell CMake to create the helloworld executable
add_executable(qlegacystyle ${qlegacystyle_SRC})

# Use the Widgets module from Qt 5.
target_link_libraries(qlegacystyle Qt5::Widgets)

# Install the executable
install(TARGETS qlegacystyle DESTINATION bin)
