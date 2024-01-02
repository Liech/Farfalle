cmake_minimum_required(VERSION 2.8...3.20)
project(FarfalleTests)

find_package(Catch2 REQUIRED)


set(Test_Files
	"BasicLineSlice.cpp"
	"TorusSlice.cpp"
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
set_property(TARGET FarfalleTests PROPERTY CXX_STANDARD 20)

target_link_libraries(FarfalleTests PRIVATE 
Catch2::Catch2
FarfalleLib
)
target_include_directories(FarfalleTests SYSTEM PUBLIC 
"${CMAKE_CURRENT_LIST_DIR}/../Source/"
)

add_custom_command(TARGET FarfalleTests POST_BUILD 
               COMMAND ${CMAKE_COMMAND} -E copy_directory 
               "${FarfalleTests_SOURCE_DIR}/TestData"              
                $<TARGET_FILE_DIR:FarfalleTests>/TestData)

include(CTest)
include(Catch)
catch_discover_tests(FarfalleTests)