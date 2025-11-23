

add_library(dependencies
        bryLexer.cpp debug.cpp
)
target_include_directories(dependencies PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(dependencies PUBLIC fmt::fmt-header-only)
add_executable(berrie main.cpp
        ../include/bryParser.h
        ../include/bryChunk.h
        ../include/bryObject.h
        bryObject.cpp
        bryChunk.cpp
        bryParser.cpp
        bryString.cpp
        bryToken.cpp)
target_compile_definitions(berrie PRIVATE PROJECT_ROOT="${CMAKE_SOURCE_DIR}")
target_link_libraries(berrie PUBLIC dependencies)