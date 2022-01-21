cmake_minimum_required(VERSION 3.14)
project(CompressTexture)
set(BINARY ${CMAKE_PROJECT_NAME})

file(GLOB_RECURSE CPP_SRC src/*.cpp src/*.h)
add_executable(${BINARY} ${CPP_SRC} )
set_property(TARGET ${BINARY} PROPERTY CXX_STANDARD 17)

if(MSVC)
    target_compile_options(${BINARY} PRIVATE /nologo /Z7 /W4 /WX /Fe)
endif()

