cmake_minimum_required(VERSION 3.0.2)
set(CMAKE_CONFIGURATION_TYPES Debug Release)
#set(CMAKE_BUILD_TYPE Debug CACHE STRING "What build type to create (Debug/Release)." FORCE)

option(BUILD_SHARED_LIBS "Build shared libraries (DLLs)." ON)

# I name thee
project(gameenginecourse)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	message("Setting up G++ stuff")
	set(CMAKE_CXX_FLAGS_DEBUG 	"${CMAKE_CXX_FLAGS_DEBUG} -g -Wno-attributes -Wno-deprecated -fexceptions")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O3")
	# Verbose output linking
	#set(CMAKE_EXE_LINKER_FLAGS 	"-v")

	add_compile_options(-lstdc++)
	add_compile_options(-std=c++1y)
	add_compile_options(-m64)
	if (WIN32)
		add_compile_options(-lws2_32)
		add_compile_options(-D_WINDOWS)
		add_compile_options(-D_WIN32)
		add_compile_options(-mwindows)
	endif(WIN32)
	set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
endif()
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	# Parallel compilation in Visual Studio
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /D \"_CRT_SECURE_NO_WARNINGS\"")

	# Ignore the "PDB not found" linker error and fix warning LNK4098: defaultlib 'msvcrt.lib' conflicts with use of other libs
	SET(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /ignore:4099 /ignore:4373 /NODEFAULTLIB:\"MSVCRT\"")
	SET(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /ignore:4099 /ignore:4373 /NODEFAULTLIB:\"MSVCRTD\"")
	SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:\"MSVCRT\"")
	SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /NODEFAULTLIB:\"MSVCRTD\"")
endif()
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CMAKE_CXX_FLAGS_DEBUG 	"${CMAKE_CXX_FLAGS_DEBUG} -g -fcolor-diagnostics -Wno-ignored-attributes -Wno-deprecated -Wno-shift-op-parentheses -fexceptions")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O3")
	set(CMAKE_LINKER 			"/usr/bin/llvm-ld")
	set(CMAKE_CXX_COMPILER 		"/usr/bin/clang++")
	set(CMAKE_C_COMPILER 		"/usr/bin/clang")
	# Verbose output linking
	#set(CMAKE_EXE_LINKER_FLAGS 	"-v")

	add_compile_options(-stdlib=libc++)
	add_compile_options(-std=c++1y)
	add_compile_options(-m64)
	set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
endif()
add_definitions(-D__extern_always_inline=inline)
add_definitions(-DGLM_FORCE_RADIANS)

string(TOLOWER ${CMAKE_SYSTEM_NAME} OperatingSystemNameLowerCase)
# Define a variable for the root of the project (Not the project file)
set(ProjectRoot ${gameenginecourse_SOURCE_DIR}/../)
get_filename_component(ProjectRootAbsolute ${ProjectRoot} ABSOLUTE)

# Include directory
file(RELATIVE_PATH IncludeDirectoryRelative ${gameenginecourse_SOURCE_DIR} ${ProjectRootAbsolute}/include)
get_filename_component(IncludeDirectoryAbsolute ${IncludeDirectoryRelative} ABSOLUTE)

# Library directory
file(RELATIVE_PATH LibraryDirectoryRelative ${gameenginecourse_SOURCE_DIR} ${ProjectRootAbsolute}/lib/${OperatingSystemNameLowerCase}/x86_64)
get_filename_component(LibraryDirectoryAbsolute ${LibraryDirectoryRelative} ABSOLUTE)

# Project settings
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug)
endif()
if(MSVC)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ProjectRoot}/bin/${OperatingSystemNameLowerCase}/)
else(MSVC)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ProjectRoot}/bin/${OperatingSystemNameLowerCase}\ ${CMAKE_CXX_COMPILER_ID}/${CMAKE_BUILD_TYPE})
endif(MSVC)

set(CMAKE_INSTALL_PREFIX "${ProjectRoot}gameenginecourse")
set(TargetArchitecture "x86_64")

# Find SDL2
find_path(SDL2Include SDL2/SDL.h
	PATHS ${IncludeDirectoryAbsolute})
set(SDL2Includes ${SDL2Include})
find_library(SDL2Library
	NAMES SDL2 SDL2main
	PATHS ${LibraryDirectoryAbsolute})
set(SDL2Libraries ${SDL2Library})
if (WIN32)
	find_library(SDL2mainLibrary
		NAMES SDL2main
		PATHS ${LibraryDirectoryAbsolute})
	list(APPEND SDL2Libraries ${SDL2mainLibrary})
endif(WIN32)

if(MSYS)
	list(APPEND SDL2Libraries "mingw32")
	list(APPEND SDL2Libraries "SDL2main")
endif(MSYS)

# Find glew
find_path(GLEWInclude GL/glew.h
	PATHS ${IncludeDirectoryAbsolute})
find_library(GLEWLibrary
	NAMES glew32 glew GLEW
	PATHS ${LibraryDirectoryAbsolute})
list(APPEND openglLibraries ${GLEWLibrary})

# Find soil
find_path(SOIL2Include soil2/SOIL2.h
	PATHS ${IncludeDirectoryAbsolute})
find_library(SOIL2Library
	NAMES soil2 SOIL2 libsoil2 soil2-debug
	PATHS ${LibraryDirectoryAbsolute})

# Find fmod
find_path(FMODInclude fmod/fmod.h
	PATHS ${IncludeDirectoryAbsolute})
find_library(FMODLibrary
	NAMES fmodex64_vc fmodexL64_vc
	PATHS ${LibraryDirectoryAbsolute})

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	list(APPEND standardLibraries "c++")
	list(APPEND standardLibraries "c++abi")
endif()
if (UNIX)
	list(APPEND standardLibraries "pthread")
	list(APPEND openglLibraries "GL")
endif (UNIX)
if (WIN32)
	list(APPEND openglLibraries "opengl32")
endif(WIN32)

include_directories(
	${SDL2Includes}
	${GLEWInclude}
	${FMODInclude}
	${IncludeDirectoryRelative}
	${ProjectRoot}/src/
)

# Input defines
set(CUSTOM_ALLOCATOR_HEADER_FOR_INPUT "<memory/Alloc.h>")
add_definitions(-DINPUT_ALLOCATION_HEADER=${CUSTOM_ALLOCATOR_HEADER_FOR_INPUT})
set(USE_CUSTOM_ALLOCATOR_HEADER_FOR_INPUT true)

# Utility defines
set(CUSTOM_ALLOCATOR_HEADER_FOR_UTILITY "<memory/Alloc.h>")
add_definitions(-DUTILITY_ALLOCATION_HEADER=${CUSTOM_ALLOCATOR_HEADER_FOR_UTILITY})
set(USE_CUSTOM_ALLOCATOR_HEADER_FOR_UTILITY true)

# Add each separate library/executable
add_subdirectory(../src/core ../build/src/core)
add_subdirectory(../src/utility ../build/src/utility)
add_subdirectory(../src/profiler ../build/src/profiler)
add_subdirectory(../src/memory ../build/src/memory)
add_subdirectory(../src/input ../build/src/input)
add_subdirectory(../src/gfx ../build/src/gfx)
