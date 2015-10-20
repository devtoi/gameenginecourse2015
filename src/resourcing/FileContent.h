#pragma once
#include <cstddef>
#include <memory/Alloc.h>

struct FileContent {
	bool Loaded = false;
	size_t Size = 0;
	pString Suffix = "";
	void* Content = nullptr;
};

#define INVALID_FILE_CONTENT FileContent()
