cmake_minimum_required(VERSION 2.8...3.20)
project(FarfalleLib)
project(Farfalle)
project(FarfalleDev) # without python copy for faster iteration

############################################################################
############################################################################
############################################################################

find_package(OpenMP)
find_package(Eigen3 CONFIG REQUIRED)
find_package(glm REQUIRED)

add_subdirectory(Lib)

############################################################################
############################################################################
############################################################################

set(Source_Files
    "Source/SliceConfig.h"
    "Source/SliceConfig.cpp"
    "Source/Geometry.h"
    "Source/Geometry.cpp"
)
source_group("Source" FILES ${Source_Files})

set(Tools_Files
    "Source/Tools/String2File.h"
    "Source/Tools/String2File.cpp"
    "Source/Tools/MovementType.h"
    "Source/Tools/glm2svg.h"
    "Source/Tools/glm2svg.cpp"
	"Source/Tools/STL.cpp"
	"Source/Tools/STL.h"
)
source_group("Source/Tools/" FILES ${Tools_Files})

set(MagickaVox_Chunk_Files
    "Source/Tools/MagicaVox/Chunk/Chunk.h"
    "Source/Tools/MagicaVox/Chunk/ChunkFactory.h"
    "Source/Tools/MagicaVox/Chunk/ChunkLAYR.h"
    "Source/Tools/MagicaVox/Chunk/ChunkLAYR.cpp"
    "Source/Tools/MagicaVox/Chunk/ChunkMAIN.h"
    "Source/Tools/MagicaVox/Chunk/ChunkMAIN.cpp"
    "Source/Tools/MagicaVox/Chunk/ChunkMATL.h"
    "Source/Tools/MagicaVox/Chunk/ChunkMATL.cpp"
    "Source/Tools/MagicaVox/Chunk/ChunknGRP.h"
    "Source/Tools/MagicaVox/Chunk/ChunknGRP.cpp"
    "Source/Tools/MagicaVox/Chunk/ChunknSHP.h"
    "Source/Tools/MagicaVox/Chunk/ChunknSHP.cpp"
    "Source/Tools/MagicaVox/Chunk/ChunknTRN.h"
    "Source/Tools/MagicaVox/Chunk/ChunknTRN.cpp"
    "Source/Tools/MagicaVox/Chunk/ChunkPACK.h"
    "Source/Tools/MagicaVox/Chunk/ChunkPACK.cpp"
    "Source/Tools/MagicaVox/Chunk/ChunkRGBA.h"
    "Source/Tools/MagicaVox/Chunk/ChunkRGBA.cpp"
    "Source/Tools/MagicaVox/Chunk/ChunkrOBJ.h"
    "Source/Tools/MagicaVox/Chunk/ChunkrOBJ.cpp"
    "Source/Tools/MagicaVox/Chunk/ChunkSIZE.h"
    "Source/Tools/MagicaVox/Chunk/ChunkSIZE.cpp"
    "Source/Tools/MagicaVox/Chunk/ChunkXYZI.h"
    "Source/Tools/MagicaVox/Chunk/ChunkXYZI.cpp"
    "Source/Tools/MagicaVox/Chunk/ChunkRCAM.h"
    "Source/Tools/MagicaVox/Chunk/ChunkRCAM.cpp"
    "Source/Tools/MagicaVox/Chunk/ChunkNOTE.h"
    "Source/Tools/MagicaVox/Chunk/ChunkNOTE.cpp"
	"Source/Tools/MagicaVox/Chunk/Chunk.cpp"
    "Source/Tools/MagicaVox/Chunk/ChunkFactory.cpp"
)
source_group("Source/Tools/MagicaVox/Chunk/" FILES ${MagickaVox_Chunk_Files})

set(MagickaVox_IO
    "Source/Tools/MagicaVox/IO/Reader.h"
    "Source/Tools/MagicaVox/IO/Reader.cpp"
    "Source/Tools/MagicaVox/IO/typeDICT.h"
    "Source/Tools/MagicaVox/IO/typeDICT.cpp"
    "Source/Tools/MagicaVox/IO/typeROTATION.h"
    "Source/Tools/MagicaVox/IO/typeROTATION.cpp"
    "Source/Tools/MagicaVox/IO/Writer.h"
    "Source/Tools/MagicaVox/IO/Writer.cpp"
)
source_group("Source/Tools/MagicaVox/IO" FILES ${MagickaVox_IO})

source_group("Source/Tools/MagicaVox" FILES ${MagickaVox_Files})
set(MagickaVox_Files
    "Source/Tools/MagicaVox/VoxFile.h"
    "Source/Tools/MagicaVox/VoxFile.cpp"
    "Source/Tools/MagicaVox/VoxFileRaw.h"
    "Source/Tools/MagicaVox/VoxFileRaw.cpp"
)
source_group("Source/Tools/MagicaVox" FILES ${MagickaVox_Files})

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
    "Source/GCode/LineMerger.h"
    "Source/GCode/LineMerger.cpp"
)
source_group("Source/GCode" FILES ${GCode_Files})

set(Voxel_Files
	"Source/Voxel/Voxelizer.cpp"
	"Source/Voxel/Voxelizer.h"
	"Source/Voxel/MarchingCubes.cpp"
	"Source/Voxel/MarchingCubes.h"
	"Source/Voxel/DistanceMap.cpp"
	"Source/Voxel/DistanceMap.h"
	"Source/Voxel/CSG.cpp"
	"Source/Voxel/CSG.h"
	"Source/Voxel/LineTracer.cpp"
	"Source/Voxel/LineTracer.h"
)
source_group("Source/Voxel" FILES ${Voxel_Files})

set(API_Files
	"Source/API/FarfalleAPI.cpp"
	"Source/API/FarfalleAPI.h"
	"Source/API/ModelAPI.cpp"
	"Source/API/ModelAPI.h"
	"Source/API/VoxelAPI.cpp"
	"Source/API/VoxelAPI.h"
	"Source/API/LineAPI.cpp"
	"Source/API/LineAPI.h"
	"Source/API/GCodeAPI.cpp"
	"Source/API/GCodeAPI.h"
	"Source/API/ScriptAPI.cpp"
	"Source/API/ScriptAPI.h"
	"Source/API/apiDatabase.cpp"
	"Source/API/apiDatabase.h"
	"Source/API/CustomPythonFunctions.cpp"
	"Source/API/CustomPythonFunctions.h"	
)
source_group("Source/API" FILES ${API_Files})


set(ALL_FILES
    ${Source_Files}
    ${Tools_Files}
	${MagickaVox_Files}
	${MagickaVox_Chunk_Files}
	${MagickaVox_IO}
    ${Voxel_Files}
    ${GCode_Files}
	${API_Files}
)

############################################################################
############################################################################
############################################################################

add_library(FarfalleLib ${ALL_FILES})
add_executable(Farfalle "Source/main.cpp")
add_executable(FarfalleDev "Source/main.cpp")
set_property(TARGET Farfalle PROPERTY CXX_STANDARD 20)
set_property(TARGET FarfalleDev PROPERTY CXX_STANDARD 20)
set_property(TARGET FarfalleLib PROPERTY CXX_STANDARD 20)


target_link_libraries(FarfalleLib
glm::glm
Eigen3::Eigen
OpenMP::OpenMP_CXX
PolyglotAPI
)
target_link_libraries(Farfalle
FarfalleLib
)
target_link_libraries(FarfalleDev
FarfalleLib
)

target_include_directories(FarfalleLib PUBLIC 
./Source/
./Lib/
)

get_filename_component(pyDir "${Python3_EXECUTABLE}" DIRECTORY )

# .\python -m ensurepip
execute_process(COMMAND ${Python3_EXECUTABLE} -m ensurepip)
#.\scripts\pip install scipy
execute_process(COMMAND "${pyDir}/scripts/pip3" install numpy)
execute_process(COMMAND "${pyDir}/scripts/pip3" install scipy)

add_custom_command(TARGET Farfalle POST_BUILD 
               COMMAND ${CMAKE_COMMAND} -E copy_directory 
               ${pyDir}              
                $<TARGET_FILE_DIR:Farfalle>/Data/python)


	

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT FarfalleDev)

add_subdirectory(Test)