cmake_minimum_required(VERSION 2.8...3.20)
project(Farfalle)

############################################################################
############################################################################
############################################################################


find_package(glm REQUIRED)
find_package(cgal REQUIRED)

############################################################################
############################################################################
############################################################################

set(Source_Files
    "Source/main.cpp"
)
source_group("Source" FILES ${Source_Files})

set(Tools_Files
    "Source/Tools/String2File.h"
    "Source/Tools/String2File.cpp"
    "Source/Tools/MovementType.h"
)
source_group("Source/Tools" FILES ${Tools_Files})

set(GCode_Files
    "Source/GCode/LinearPrint.h"
    "Source/GCode/LinearPrint.cpp"
    "Source/GCode/Travel.h"
    "Source/GCode/Travel.cpp"
    "Source/GCode/Printer.h"
    "Source/GCode/Printer.cpp"
    "Source/GCode/Temperature.h"
    "Source/GCode/Temperature.cpp"
    "Source/GCode/Movement.h"
    "Source/GCode/Movement.cpp"
    "Source/GCode/Extruder.h"
    "Source/GCode/Extruder.cpp"
)
source_group("Source/GCode" FILES ${GCode_Files})

set(Geometry_Files
    "Source/Geometry/Model.h"
    "Source/Geometry/Model.cpp"
)
source_group("Source/Geometry" FILES ${Geometry_Files})

set(ALL_FILES
    ${Source_Files}
    ${Tools_Files}
    ${GCode_Files}
    ${Geometry_Files}
)

############################################################################
############################################################################
############################################################################

add_executable(Farfalle ${ALL_FILES})
set_property(TARGET Farfalle PROPERTY CXX_STANDARD 20)


target_link_libraries(Farfalle 
glm::glm
CGAL::CGAL
)

target_include_directories(Farfalle PRIVATE 
./Source/
)

#add_dependencies(AssimpSupCom 
#)
	

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Farfalle)