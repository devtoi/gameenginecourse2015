#pragma once
#include <cstddef>
#include <memory/Alloc.h>

struct FileContent {
	bool Loaded = false;
	size_t Size = 0;
	pString Suffix = "";
	void* Content = nullptr;
	size_t ID = 0;
};

#define INVALID_FILE_CONTENT FileContent()
