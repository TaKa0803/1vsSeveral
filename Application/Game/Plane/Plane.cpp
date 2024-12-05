#include "Plane.h"
#include<imgui.h>


Plane::Plane() {
	GameObject::Initialize("plane");
}

Plane::~Plane()
{
	
}

void Plane::Initialize() {
	
	world_.scale_ = { 500,500,500 };

	world_.UpdateMatrix();
}

void Plane::DebagWindow() {

	
}

void Plane::Draw() {
	world_.UpdateMatrix();
	GameObject::Draw();
}
