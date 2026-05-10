cmake_minimum_required(VERSION 3.10)
project(hexapod)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CINDER_PATH "$ENV{HOME}/Repos/Cinder" CACHE PATH "Cinder root")
include("${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake")

find_package(Bullet REQUIRED)

ci_make_app(
    APP_NAME    hexapod
    SOURCES     src/HexapodApp.cpp
                src/Leg.cpp
                src/HexapodBody.cpp
    CINDER_PATH ${CINDER_PATH}
)

target_include_directories(hexapod PRIVATE ${BULLET_INCLUDE_DIRS})
target_link_libraries(hexapod PRIVATE ${BULLET_LIBRARIES})
