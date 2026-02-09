#pragma once
#include "DxLib.h"

class Player {
private:
	float x, y;
	float speed;
	int handle;
	int flip_flag;

	int footSE = -1; //足音
	
	// スプライトシート用の変数
	int spriteSheetWidth;
	int spriteSheetHeight;
	int spriteWidth;
	int spriteHeight;
	float animationTimer;
	int animationFrame;
	bool isMoving;

public:
	Player();
	void Initialize();
	void Update();
	void Draw(float cameraOffsetX = 0.0f);

	// 足音を停止させる関数
	void StopAudio();

	float GetX() const { return x; }
	float GetY() const { return y; }

	// X座標を強制設定する関数（エリア移動で使用）
	void SetX(float newX) { x = newX; }
};