#!/bin/bash
if [ $# -lt 2 ] && [ $# -gt 3 ]; then
	printf "Usage: `basename $0` type(dense/sparse) name [location]"
	exit 65
fi

if [ "$1" == "dense" ] || [ "$1" == "Dense" ] || [ "$1" == "d" ]; then
	ofType="template class DenseBackend<${2}Component>;"
	typeInclude="Dense"
elif [ $1 == "sparse" ] || [ $2 == "Sparse" ] || [ "$1" == "s" ]; then
	ofType="template class SparseBackend<${2}Component>;"
	typeInclude="Sparse"
else
	printf "Invalid type: %s. Valid types are (d)ense and (s)parse\n" "$1"
	exit 65
fi

componentName=$2
if [ $# -lt 3 ]; then
	componentDirectory="core/component"
	printf "Using default directory as none was given: %s\n" "$componentDirectory"
else
	componentDirectory=$3
fi
headerFile="#pragma once

template<typename Component>
class ${typeInclude}Backend;

struct ${componentName}Component {
	static const char* ComponentName; // Name for the component type. Will be initialized in cpp file.
	static int ComponentID;
	typedef ${typeInclude}Backend<${2}Component> Backend;
};
"
cppFile="#include \"${componentName}Component.h\"
#include \"../dataoriented/ComponentCollection.cpp\"
#include \"../dataoriented/${typeInclude}Backend.cpp\"
const char* ${componentName}Component::ComponentName = \"${componentName}\";
int ${componentName}Component::ComponentID = -1;
$ofType
template class ComponentCollection<${2}Component>;
"
# Create directory if it does not exist
mkdir -p ${componentDirectory}

headerFilePath="${componentDirectory}/${componentName}Component.h"
cppFilePath="${componentDirectory}/${componentName}Component.cpp"
printf "Creating header file %s\n" "$headerFilePath"
printf "Creating cpp file %s\n" "$cppFilePath"

printf "%s" "$headerFile" > $headerFilePath
printf "%s" "$cppFile" > $cppFilePath
