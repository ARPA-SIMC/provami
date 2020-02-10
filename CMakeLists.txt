project(provami)

cmake_minimum_required(VERSION 2.8.11)

set(PACKAGE_VERSION "1.7")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Concurrent REQUIRED)
find_package(Qt5WebKitWidgets REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(DBALLE REQUIRED libdballe>=8.8)

include_directories(${CMAKE_SOURCE_DIR})

include_directories(${DBALLE_INCLUDE_DIRS})
link_directories(${DBALLE_LIBRARY_DIRS})
add_definitions(${DBALLE_CFLAGS_OTHER})

set(DATADIR "${CMAKE_INSTALL_PREFIX}/share/provami")

add_definitions(-DDATADIR="${DATADIR}" -DPACKAGE_VERSION="${PACKAGE_VERSION}")

#file(GLOB provami_headers provami/*.h)
file(GLOB provami_uis provami/*.ui)
file(GLOB provami_libsrc provami/*.cpp)
file(GLOB provami_cmdsrc src/*.cpp)

# This seems to only be needed only on Fedora 20
QT5_WRAP_UI(provami_uis_HEADERS ${provami_uis})

add_executable(provami-qt ${provami_libsrc} ${provami_cmdsrc} ${provami_uis})

target_link_libraries(provami-qt Qt5::Widgets Qt5::WebKitWidgets)
target_link_libraries(provami-qt ${DBALLE_LIBRARIES})

if(UNIX)
    add_definitions(-Wall)
endif(UNIX)

install(TARGETS provami-qt DESTINATION bin)
install(DIRECTORY mapview DESTINATION ${DATADIR})
# improvable (see https://github.com/htacg/tidy-html5/issues/193)
install(FILES provami-qt.1 DESTINATION ${CMAKE_INSTALL_PREFIX}/share/man/man1)
