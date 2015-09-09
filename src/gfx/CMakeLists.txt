file(GLOB_RECURSE GFXSources
	"*.h"
	"*.cpp"
	"*/*.h"
	"*/*.cpp"
)
add_definitions(-DGFX_DLL_EXPORT)
add_library(gfx SHARED ${GFXSources})
target_link_libraries(gfx Utility Input Profiler Memory ${openglLibraries} ${SDL2Library}) # TODOJM: Remove dependency on Input

install(
	TARGETS gfx DESTINATION lib
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin/${OperatingSystemNameLowerCase}/${TargetArchitecture})
