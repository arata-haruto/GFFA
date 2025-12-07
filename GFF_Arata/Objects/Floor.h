#pragma once
#include "DxLib.h"

class Floor {
private:
	int handle;
	float x, y;
	float width, height;

public:
	Floor();
	void Initialize();
	void Draw(float cameraOffsetX = 0.0f);
	float GetY() const { return y; }
};