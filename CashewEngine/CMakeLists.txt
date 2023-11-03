# The cmake file for the CashewEngine
file(GLOB SOURCE_FILES "src/*.cpp" "src/*.h")


add_library(CashewEngine ${SOURCE_FILES})

target_include_directories(CashewEngine PUBLIC
			   "TPD/spdlog/include")
target_compile_definitions(CashewEngine PRIVATE
    $<$<CONFIG:Debug>:CASHEW_DEBUG>
    CASHEW_BUILD_DLL
)

if(WIN32)
  target_compile_definitions(Application PRIVATE CASHEW_PLATFORM_WINDOWS)
endif()

install (TARGETS CashewEngine DESTINATION bin)