
cmake_minimum_required(VERSION 3.14.5)
project(cgrep VERSION 1.1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(LLVM_CONF "" CACHE STRING "set the actual name of llvm-config, i.e. llvm-config-10")

set(LLVM_CONF_TMP "${LLVM_CONF}")
unset(LLVM_CONF CACHE)

if(LLVM_CONF_TMP STREQUAL "")
  # If LLVM_CONF wasn't provided, search for it.
  find_program(LLVM_CONF NAMES llvm-config llvm-config-12 llvm-config-11 llvm-config-10 REQUIRED)
else()
  # If LLVM_CONF was provided, check if the executable actually exists.
  find_program(LLVM_CONF NAMES "${LLVM_CONF_TMP}" REQUIRED)
endif()
unset(LLVM_CONF_TMP)

function(CleanMessage)
  execute_process(COMMAND ${CMAKE_COMMAND} -E echo "${ARGN}")
endfunction()

execute_process(COMMAND ${LLVM_CONF} --includedir OUTPUT_VARIABLE LLVM_INC_DIR)
string(REGEX REPLACE "\n$" "" LLVM_INC_DIR "${LLVM_INC_DIR}")
execute_process(COMMAND ${LLVM_CONF} --cxxflags OUTPUT_VARIABLE LLVM_CXX_FLAGS)
string(REGEX REPLACE "\n$" "" LLVM_CXX_FLAGS "${LLVM_CXX_FLAGS}")
execute_process(COMMAND ${LLVM_CONF} --libdir OUTPUT_VARIABLE LLVM_LIB_DIR)
string(REGEX REPLACE "\n" "" LLVM_LIB_DIR "${LLVM_LIB_DIR}")
execute_process(COMMAND ${LLVM_CONF} --ldflags OUTPUT_VARIABLE LLVM_LD_FLAGS)
string(REGEX REPLACE "\n" "" LLVM_LD_FLAGS "${LLVM_LD_FLAGS}")
execute_process(COMMAND ${LLVM_CONF} --libs OUTPUT_VARIABLE LLVM_LIBS)
string(REGEX REPLACE "\n$" "" LLVM_LIBS "${LLVM_LIBS}")
string(REGEX REPLACE "^-l" "" LLVM_LIBS "${LLVM_LIBS}")
execute_process(COMMAND ${LLVM_CONF} --system-libs OUTPUT_VARIABLE LLVM_SYS_LIBS)
string(REGEX REPLACE "\n$" "" LLVM_SYS_LIBS "${LLVM_SYS_LIBS}")
string(REGEX REPLACE "^-l" "" LLVM_SYS_LIBS "${LLVM_SYS_LIBS}")

add_compile_options(${LLVM_CXX_FLAGS})
add_compile_options(-I${LLVM_INC_DIR})

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_link_options(${LLVM_LD_FLAGS})
add_link_options(-L${LLVM_LIB_DIR})

add_executable(cgrep cgrep.cpp ${CMAKE_SOURCE_DIR}/cfe-extra/cfe_extra.cpp)
option(USE_MONOLITH_LIBTOOLING "use libtooling built into a single library" OFF)
if (USE_MONOLITH_LIBTOOLING)
  target_link_libraries(cgrep clang-cpp)
else()
  target_link_libraries(cgrep -Wl,--start-group clangAST clangAnalysis clangBasic clangDriver clangEdit clangFrontend clangFrontendTool clangLex clangParse clangSema clangEdit clangASTMatchers clangRewrite clangRewriteFrontend clangStaticAnalyzerFrontend clangStaticAnalyzerCheckers clangStaticAnalyzerCore clangSerialization clangToolingCore clangTooling stdc++ LLVMRuntimeDyld m -Wl,--end-group)
endif()
target_link_libraries(cgrep ${LLVM_SYS_LIBS})
target_link_libraries(cgrep ${LLVM_LIBS})

if(LLVM_PACKAGE_VERSION VERSION_EQUAL "15.0.0" OR LLVM_PACKAGE_VERSION VERSION_GREATER "15.0.0")
  target_link_libraries(cgrep clangSupport)
endif()

include_directories("${PROJECT_SOURCE_DIR}/cfe-extra")
target_include_directories(cgrep PUBLIC "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR/cfe-extra}")

