cmake_minimum_required(VERSION 2.8...3.20)
project(Farfalle)

############################################################################
############################################################################
############################################################################


find_package(glm REQUIRED)

############################################################################
############################################################################
############################################################################

set(Header_Files
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "Source/main.cpp"
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
	

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Farfalle)