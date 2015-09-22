# Recursivly add source files in the subfolders selected
file(GLOB_RECURSE SOURCES
	"camera/*.h"
	"camera/*.cpp"
	"dataoriented/*.h"
	"dataoriented/*.cpp"
	"subsystem/*.h"
	"subsystem/*.cpp"
	"component/*.h"
	"component/*.cpp"
	"utility/*.h"
	"utility/*.cpp"
	"Engine.h"
	"Engine.cpp"
)

# Generate source groups depending on folders (creates Visual Studio filters)
set(RELPATH ${ProjectRoot}/src/)
get_filename_component(RELPATH ${RELPATH} ABSOLUTE)
foreach(f ${SOURCES})
    # Get the path of the file relative to ${DIRECTORY},
    # then alter it (not compulsory)
    file(RELATIVE_PATH SRCGR ${RELPATH} ${f})
    # Extract the folder, ie remove the filename part
    string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" SRCGR ${SRCGR})

    # Source_group expects \\ (double antislash), not / (slash)
    string(REPLACE / \\ SRCGR ${SRCGR})
    source_group("${SRCGR}" FILES ${f})
endforeach()


SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 

SET(CMAKE_INSTALL_RPATH
	"$ORIGIN/../../../lib/")

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

# Make executable from main.cpp and sources listed above
add_executable(${CMAKE_PROJECT_NAME} Main.cpp ${SOURCES})
# Link libraries to the executable
target_link_libraries(${CMAKE_PROJECT_NAME} Utility gfx Profiler Input Memory ${SDL2Libraries} ${standardLibraries} ${FMODLibrary})

MOVEDLL(${CMAKE_PROJECT_NAME} SDL2)
MOVEDLL(${CMAKE_PROJECT_NAME} glew32)
MOVEDLL(${CMAKE_PROJECT_NAME} fmodex64)

install(TARGETS ${CMAKE_PROJECT_NAME} RUNTIME DESTINATION bin/${OperatingSystemNameLowerCase}/${TargetArchitecture})
install(DIRECTORY ${ProjectRoot}shader DESTINATION .)
if (MSVC)
	install(DIRECTORY ${ProjectRoot}sharedlibs/windows/${CMAKE_BUILD_TYPE}/ DESTINATION bin/${OperatingSystemNameLowerCase}/${TargetArchitecture})
endif(MSVC)
if (UNIX)
	install(DIRECTORY ${ProjectRoot}sharedlibs/linux/ DESTINATION lib)
endif(UNIX)
install(DIRECTORY ${ProjectRoot}cfg DESTINATION .)
install(DIRECTORY ${ProjectRoot}asset DESTINATION .)
install(DIRECTORY ${ProjectRoot}lib/licenses DESTINATION .)
