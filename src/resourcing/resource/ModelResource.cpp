#include "ModelResource.h"

ModelResource::ModelResource() {
}

ModelResource::~ModelResource() {

}

void ModelResource::CalcSize() {
	this->SetSize(sizeof(this));
}