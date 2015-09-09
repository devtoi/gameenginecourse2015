#include "PositionComponent.h"
#include "../dataoriented/ComponentCollection.cpp"
#include "../dataoriented/DenseBackend.cpp"
const char* PositionComponent::ComponentName = "Position";
int PositionComponent::ComponentID = -1;
template class DenseBackend<PositionComponent>;
template class ComponentCollection<PositionComponent>;
