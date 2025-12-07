#include "Player.h"
#include "../Utility/InputManager.h"

Player::Player() {
	x = 320.0f;
	y = 580.0f;
	speed = 4.0f;
	handle = -1;
	flip_flag = FALSE;
}

void Player::Initialize() {
	handle = LoadGraph("Resource/Characters/Player/Player.png");
	footSE = LoadSoundMem("Resource/Sound/foot.mp3");
}

void Player::Update() {
	InputManager* input = InputManager::GetInstance();

	bool isMoving = false;

	if (input->GetKeyState(KEY_INPUT_RIGHT) == eInputState::Pressed ||
		input->GetKeyState(KEY_INPUT_RIGHT) == eInputState::Held ||
		input->GetButtonState(XINPUT_BUTTON_DPAD_RIGHT) == eInputState::Pressed ||
		input->GetButtonState(XINPUT_BUTTON_DPAD_RIGHT) == eInputState::Held)
	{
		x += speed;
		flip_flag = FALSE;
		isMoving = true;
	}

	if (input->GetKeyState(KEY_INPUT_LEFT) == eInputState::Pressed ||
		input->GetKeyState(KEY_INPUT_LEFT) == eInputState::Held ||
		input->GetButtonState(XINPUT_BUTTON_DPAD_LEFT) == eInputState::Pressed ||
		input->GetButtonState(XINPUT_BUTTON_DPAD_LEFT) == eInputState::Held)
	{
		x -= speed;
		flip_flag = TRUE;
		isMoving = true;
	}

	if (footSE != -1) {
		static bool isPlaying = false;

		if (isMoving) {
			if (!isPlaying) {
				PlaySoundMem(footSE, DX_PLAYTYPE_LOOP, TRUE);
				isPlaying = true;
			}
		}
		else {
			if (isPlaying) {
				StopSoundMem(footSE);
				isPlaying = false;
			}
		}
	}

	const float width = 150.0f;
	const float height = 225.0f;
	const float screenW = 1280.0f;
	const float screenH = 720.0f;

	// ★変更点: マップ幅を広げる（4000 -> 8000）
	const float mapWidth = 8000.0f;

	if (x < 0) x = 0;
	if (x > mapWidth - width) x = mapWidth - width;

	if (y < 0) y = 0;
	if (y > screenH - height) y = screenH - height;

	const float floorY = 600.0f;
	const float playerHeight = 225.0f;

	if (y > floorY - playerHeight) {
		y = floorY - playerHeight;
	}
}

void Player::Draw(float cameraOffsetX) {
	float drawX = x - cameraOffsetX;
	if (flip_flag) {
		DrawTurnGraph((int)drawX, (int)y, handle, TRUE);
	}
	else {
		DrawGraph((int)drawX, (int)y, handle, TRUE);
	}
}

void Player::StopAudio() {
	if (footSE != -1 && CheckSoundMem(footSE)) {
		StopSoundMem(footSE);
	}
}