project(provami)

cmake_minimum_required(VERSION 2.8.11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(Qt5Widgets)
find_package(Qt5WebKitWidgets)

find_package(PkgConfig REQUIRED)
pkg_check_modules(DBALLE REQUIRED libdballe>=7.0)

include_directories(${CMAKE_SOURCE_DIR})

include_directories(${DBALLE_INCLUDE_DIRS})
link_directories(${DBALLE_LIBRARY_DIRS})
add_definitions(${DBALLE_CFLAGS_OTHER})

set(DATADIR "${CMAKE_INSTALL_PREFIX}/share/provami")

#add_definitions(${QT_DEFINITIONS})
add_definitions(-DDATADIR="${DATADIR}")

#file(GLOB provami_headers provami/*.h)
#file(GLOB provami_uis provami/*.ui)
file(GLOB provami_libsrc provami/*.cpp)
file(GLOB provami_cmdsrc src/*.cpp)
file(GLOB provami_testsrc tests/*.cpp)

add_executable(provami-qt ${provami_libsrc} ${provami_cmdsrc})
add_executable(test-provami ${provami_libsrc} ${provami_testsrc})

target_link_libraries(provami-qt Qt5::Widgets Qt5::WebKitWidgets)
target_link_libraries(provami-qt ${DBALLE_LIBRARIES})
target_link_libraries(test-provami Qt5::Widgets Qt5::WebKitWidgets)
target_link_libraries(test-provami ${DBALLE_LIBRARIES})

if(UNIX)
    add_definitions(-Wall)
endif(UNIX)

enable_testing()
add_test(NAME test-provami COMMAND test-provami)

install(TARGETS provami-qt DESTINATION bin)
install(DIRECTORY mapview DESTINATION ${DATADIR})

# DISTFILES += mapview
# include(defines.inc)
# data.path = $$PREFIX/share/provami
# data.files = mapview
# INSTALLS += data
