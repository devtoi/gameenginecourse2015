#pragma once

class Resource;

class ResourceLoader {
	virtual Resource* LoadResource() = 0;
};
