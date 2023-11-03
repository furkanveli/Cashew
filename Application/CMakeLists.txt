# The top cmake file for the solution
cmake_minimum_required(2.6)
project(Application)

#The version number
set (Cashew_VERSION_MAJOR 1)
set (Cashew_VERSION_MINOR 0)
set (SOLUTION_DIR "${CMAKE_SOURCE_DIR}/.." PARENT_SCOPE)
file(GLOB SOURCE_FILES "src/*.cpp" "src/*.h")

target_include_directories(Application PUBLIC
			   "SOLUTION_DIR/CashewEngine/src"
			   "SOLUTION_DIR/CashewEngine/TPD/spdlog/include"
				)
target_compile_definitions(Application PRIVATE $<$<CONFIG:Debug>:CASHEW_DEBUG)
if(WIN32)
  target_compile_definitions(Application PRIVATE CASHEW_PLATFORM_WINDOWS)
endif()

add_subdirectory(CashewEngine)
add_executable(Application ${SOURCE_FILES})
target_link_libraries(Application PUBLIC CashewEngine)

install (TARGETS Application DESTINATION bin)