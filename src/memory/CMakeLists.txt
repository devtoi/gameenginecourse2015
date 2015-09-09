file(GLOB_RECURSE MemorySources
	"*.h"
	"*.cpp"
)
add_definitions(-DMEMORY_DLL_EXPORT)
add_library(Memory SHARED ${MemorySources})

install(
	TARGETS Memory DESTINATION lib
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin/${OperatingSystemNameLowerCase}/${TargetArchitecture})
