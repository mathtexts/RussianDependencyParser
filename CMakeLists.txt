cmake_minimum_required (VERSION 2.6)

set (PROJECT syntaxanalyzer)
project (${PROJECT})
add_definitions (-g -Wall -pedantic -std=c++0x)

set (SOURCES_GENDICT
        gendict.cpp paradigm.h paradigm.cpp)
set (SOURCES_MAIN
        model.cpp model.h main.cpp paradigm.h paradigm.cpp)

find_package (Qt4 REQUIRED)
set(QT_DONT_USE_QTGUI false)
include (${QT_USE_FILE})

add_executable (gendict ${SOURCES_GENDICT})
target_link_libraries (gendict)

add_executable (main ${SOURCES_MAIN})
target_link_libraries (main ${QT_LIBRARIES})
