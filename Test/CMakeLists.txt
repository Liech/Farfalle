cmake_minimum_required(VERSION 2.8...3.20)
project(FarfalleTests)

find_package(Catch2 REQUIRED)


set(Test_Files
	"slice.cpp"
)
source_group("Main" FILES ${Main_Files})
set(Main_Files
	"testMain.cpp"
)
source_group("Main" FILES ${Main_Files})

set(ALL_FILES
    ${Main_Files}
    ${Test_Files}
)


add_executable(FarfalleTests ${ALL_FILES})

target_link_libraries(FarfalleTests PRIVATE 
Catch2::Catch2
FarfalleLib
)
target_include_directories(FarfalleTests SYSTEM PUBLIC 
"${CMAKE_CURRENT_LIST_DIR}/../Source/"
)

#include(CTest)
#include(Catch)
#catch_discover_tests(FarfalleTests)