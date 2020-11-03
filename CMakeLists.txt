
cmake_minimum_required(VERSION 3.14.5)
project(cgrep VERSION 1.1)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(LLVM_CONF llvm-config)

function(CleanMessage)
  execute_process(COMMAND ${CMAKE_COMMAND} -E echo "${ARGN}")
endfunction()

execute_process(COMMAND LLVM_CONF --cxxflags OUTPUT_VARIABLE LLVM_CXX_FLAGS)
string(REGEX REPLACE "\n$" "" LLVM_CXX_FLAGS "${LLVM_CXX_FLAGS}")
execute_process(COMMAND LLVM_CONF --src-root OUTPUT_VARIABLE LLVM_SRC_ROOT)
string(REGEX REPLACE "\n$" "" LLVM_SRC_ROOT "${LLVM_SRC_ROOT}")
execute_process(COMMAND LLVM_CONF --obj-root OUTPUT_VARIABLE LLVM_OBJ_ROOT)
string(REGEX REPLACE "\n$" "" LLVM_OBJ_ROOT "${LLVM_OBJ_ROOT}")
execute_process(COMMAND LLVM_CONF --ldflags OUTPUT_VARIABLE LLVM_LD_FLAGS)
string(REGEX REPLACE "\n" "" LLVM_LD_FLAGS "${LLVM_LD_FLAGS}")
execute_process(COMMAND LLVM_CONF --libs OUTPUT_VARIABLE LLVM_LIBS)
string(REGEX REPLACE "\n$" "" LLVM_LIBS "${LLVM_LIBS}")
string(REGEX REPLACE "^-l" "" LLVM_LIBS "${LLVM_LIBS}")
execute_process(COMMAND LLVM_CONF --system-libs OUTPUT_VARIABLE LLVM_SYS_LIBS)
string(REGEX REPLACE "\n$" "" LLVM_SYS_LIBS "${LLVM_SYS_LIBS}")
string(REGEX REPLACE "^-l" "" LLVM_SYS_LIBS "${LLVM_SYS_LIBS}")

add_compile_options(${LLVM_CXX_FLAGS})
add_compile_options(-I"${LLVM_SRC_ROOT}/tools/clang/include")
add_compile_options(-I"${LLVM_OBJ_ROOT}/tools/clang/include")

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_link_options(${LLVM_LD_FLAGS})

add_executable(cgrep cgrep.cpp ${CMAKE_SOURCE_DIR}/cfe-extra/cfe_extra.cpp)
option(USE_MONOLITH_LIBTOOLING "use libtooling built into a single library" OFF)
if (USE_MONOLITH_LIBTOOLING)
  target_link_libraries(cgrep libclang-cpp)
else()
  target_link_libraries(cgrep -Wl,--start-group clangAST clangAnalysis clangBasic clangDriver clangEdit clangFrontend clangFrontendTool clangLex clangParse clangSema clangEdit clangASTMatchers clangRewrite clangRewriteFrontend clangStaticAnalyzerFrontend clangStaticAnalyzerCheckers clangStaticAnalyzerCore clangSerialization clangToolingCore clangTooling stdc++ LLVMRuntimeDyld m -Wl,--end-group)
endif()
target_link_libraries(cgrep ${LLVM_SYS_LIBS})
target_link_libraries(cgrep ${LLVM_LIBS})

include_directories("${PROJECT_SOURCE_DIR}/cfe-extra")
target_include_directories(cgrep PUBLIC "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR/cfe-extra}")


