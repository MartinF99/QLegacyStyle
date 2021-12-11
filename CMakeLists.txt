cmake_minimum_required(VERSION 3.16)
project(QLegacyStyle)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH}  ${CMAKE_SOURCE_DIR}/cmake)



# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# Instruct CMake to create code from Qt designer ui files
set(CMAKE_AUTOUIC ON)

# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)

set(qlegacystyle_SRC
  src/qlegacystyle.h
  src/qlegacystyle.cpp
  src/qlegacystyleplugin.cpp
  src/drawutil.h
  src/drawutil.cpp
  src/bitmaps.h
  src/qlegacystyleplugin.h
  )

set(widgetgallery_SRC
    src/widgetgallery.h
    src/widgetgallery.cpp
    src/main.cpp
    )
# Tell CMake to create a library
add_library(qlegacystyle MODULE ${qlegacystyle_SRC})

# Use the Widgets module from Qt 5.
target_link_libraries(qlegacystyle Qt::Core Qt::Gui Qt::Widgets)

# tell Cmake to add an executable
add_executable(widgetgallery ${widgetgallery_SRC})

target_link_libraries(widgetgallery PRIVATE Qt5::Widgets)

get_target_property(REAL_QMAKE_EXECUTABLE ${Qt5Core_QMAKE_EXECUTABLE} IMPORTED_LOCATION)
execute_process(COMMAND "${REAL_QMAKE_EXECUTABLE}" -query QT_INSTALL_PLUGINS
  OUTPUT_VARIABLE _Qt5_PLUGIN_INSTALL_DIR
  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
install (TARGETS qlegacystyle LIBRARY DESTINATION ${_Qt5_PLUGIN_INSTALL_DIR}/styles/)
