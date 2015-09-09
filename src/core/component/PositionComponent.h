#pragma once

template<typename Component>
class DenseBackend;

struct PositionComponent {
	static const char* ComponentName; // Name for the component type. Will be initialized in cpp file.
	static int ComponentID;
	typedef DenseBackend<PositionComponent> Backend;
};
