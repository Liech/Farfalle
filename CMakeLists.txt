cmake_minimum_required(VERSION 2.8...3.20)
project(Farfalle)

find_package(glm REQUIRED)

############################################################################
############################################################################
############################################################################

set(Header_Files
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "main.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
)

############################################################################
############################################################################
############################################################################

add_executable(Farfalle ${ALL_FILES})
set_property(TARGET Farfalle PROPERTY CXX_STANDARD 20)


target_link_libraries(Farfalle 
glm::glm
)

#target_include_directories(AssimpSupCom PRIVATE 
#)

#add_dependencies(AssimpSupCom 
#)
				
add_custom_command(TARGET AssimpSupCom POST_BUILD 
               COMMAND ${CMAKE_COMMAND} -E copy_if_different
               "${AhwassaGraphicsLib_SOURCE_DIR}/Data/Fonts/arial.ttf"              
                $<TARGET_FILE_DIR:AssimpSupCom>/Data/Fonts/arial.ttf)


set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Farfalle)