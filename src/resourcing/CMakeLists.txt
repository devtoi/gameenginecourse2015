file(GLOB_RECURSE ResourcingSources
	"ResourcingLibraryDefine.h"
	"ResourceTypes.h"
	"ResourceManager.h"
	"ResourceManager.cpp"
	"Resource.h"
	"Resource.cpp"
	"ResourceLoader.h"
	"ResourceLoader.cpp"
	"*.h"
	"*.cpp"
)

set(RESOURCING_MEMORY_LIB)
if(USE_CUSTOM_ALLOCATOR_HEADER_FOR_RESOURCING)
	set(RESOURCING_MEMORY_LIB Memory)
	add_definitions(-DRESOURCING_ALLOCATION_HEADER=${CUSTOM_ALLOCATOR_HEADER_FOR_RESOURCING})
else(USE_CUSTOM_ALLOCATOR_HEADER_FOR_RESOURCING)
	list(APPEND ResourcingSources "Alloc.h")
	add_definitions(-DRESOURCING_ALLOCATION_HEADER="Alloc.h")
endif(USE_CUSTOM_ALLOCATOR_HEADER_FOR_RESOURCING)

add_definitions(-DRESOURCING_DLL_EXPORT)
# Create library for utilities
add_library(Resourcing SHARED ${ResourcingSources})
target_link_libraries(Resourcing Utility ${RESOURCING_MEMORY_LIB} ${standardLibraries} ${openglLibraries} ${ZZIPLibrary} ${ZLIBLibrary})

install(
	TARGETS Resourcing DESTINATION lib
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin/${OperatingSystemNameLowerCase}/${TargetArchitecture})
