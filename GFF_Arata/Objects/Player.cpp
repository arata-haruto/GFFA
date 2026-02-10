#include "Player.h"
#include "../Utility/InputManager.h"
#include "../Utility/ResourceManager.h"
#include "../Scene/InGame/MapData.h"

Player::Player() {
	x = 0.0f;
	y = 700.0f;
	speed = 4.0f;
	handle = -1;
	flip_flag = FALSE; // FALSE = 右向き（初期状態は右向き）
	animationTimer = 0.0f;
	animationFrame = 3; // 初期状態は静止（右下のスプライト）
	isMoving = false;
	spriteSheetWidth = 0;
	spriteSheetHeight = 0;
	spriteWidth = 0;
	spriteHeight = 0;
}

void Player::Initialize() {
	ResourceManager* rm = ResourceManager::GetInstance();
	handle = rm->GetImageResource("Resource/Characters/Player/player.png")[0];
	footSE = rm->GetSoundResource("Resource/Sound/foot.mp3");
	
	// スプライトシートのサイズを取得（2x2グリッド）
	if (handle != -1) {
		GetGraphSize(handle, &spriteSheetWidth, &spriteSheetHeight);
		spriteWidth = spriteSheetWidth / 2;
		spriteHeight = spriteSheetHeight / 2;
	}
}

void Player::Update() {
	InputManager* input = InputManager::GetInstance();

	isMoving = false;
	bool movingRight = false;
	bool movingLeft = false;

	// 右方向の移動チェック
	if (input->GetKeyState(KEY_INPUT_RIGHT) == eInputState::Pressed ||
		input->GetKeyState(KEY_INPUT_RIGHT) == eInputState::Held ||
		input->GetButtonState(XINPUT_BUTTON_DPAD_RIGHT) == eInputState::Pressed ||
		input->GetButtonState(XINPUT_BUTTON_DPAD_RIGHT) == eInputState::Held)
	{
		x += speed;
		movingRight = true;
		isMoving = true;
	}

	// 左方向の移動チェック
	if (input->GetKeyState(KEY_INPUT_LEFT) == eInputState::Pressed ||
		input->GetKeyState(KEY_INPUT_LEFT) == eInputState::Held ||
		input->GetButtonState(XINPUT_BUTTON_DPAD_LEFT) == eInputState::Pressed ||
		input->GetButtonState(XINPUT_BUTTON_DPAD_LEFT) == eInputState::Held)
	{
		x -= speed;
		movingLeft = true;
		isMoving = true;
	}

	// 向きを設定（移動中のみ更新、移動が止まったら最後の向きを保持）
	if (movingRight) {
		flip_flag = FALSE; // 右向き
	}
	else if (movingLeft) {
		flip_flag = TRUE; // 左向き
	}
	// 移動していない場合は、flip_flagは前回の値を保持（向きを維持）

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

	const float screenW = 1280.0f;
	const float screenH = 720.0f;

	const float mapWidth = (float)MapData::MAP_WIDTH;

	const float floorY = 600.0f;
	
	// 実際のスプライトサイズを使用
	float playerWidth = (spriteWidth > 0) ? (float)spriteWidth : 150.0f;
	float playerHeight = (spriteHeight > 0) ? (float)spriteHeight : 225.0f;

	// X座標の制限
	if (x < 0) x = 0;
	if (x > mapWidth - playerWidth) x = mapWidth - playerWidth;

	// Y座標：地面の上端（600）に足がつくように設定
	// DrawRectRotaGraph2の基準点は中心なので、スプライトの下端が600になるように
	// yはスプライトの中心のY座標として設定
	y = floorY - playerHeight / 2.0f;

	// 画面外に出ないように制限
	if (y < playerHeight / 2.0f) y = playerHeight / 2.0f;
	if (y > screenH - playerHeight / 2.0f) y = screenH - playerHeight / 2.0f;
	
	// アニメーション更新
	if (isMoving) {
		animationTimer += 0.15f; // アニメーション速度調整
		if (animationTimer >= 1.0f) {
			animationTimer = 0.0f;
			animationFrame = (animationFrame + 1) % 3; // 0, 1, 2の3フレーム（歩行アニメーション）
		}
	}
	else {
		animationFrame = 3; // 静止状態（右下のスプライト）
		animationTimer = 0.0f;
	}
}

void Player::Draw(float cameraOffsetX) {
	float drawX = x - cameraOffsetX;
	
	// スプライトシートから適切な部分を切り出して描画
	if (handle != -1 && spriteWidth > 0 && spriteHeight > 0) {
		// スプライトシートの位置を計算（2x2グリッド）
		// 0: 左上(0,0), 1: 右上(1,0), 2: 左下(0,1), 3: 右下(1,1)
		int srcX = 0;
		int srcY = 0;
		
		if (animationFrame == 0) {
			// 左上（歩行1）
			srcX = 0;
			srcY = 0;
		}
		else if (animationFrame == 1) {
			// 右上（歩行2）
			srcX = spriteWidth;
			srcY = 0;
		}
		else if (animationFrame == 2) {
			// 左下（歩行3）
			srcX = 0;
			srcY = spriteHeight;
		}
		else {
			// 右下（静止状態）
			srcX = spriteWidth;
			srcY = spriteHeight;
		}
		
		// 描画位置：Y座標は既に中心座標として計算されている
		float drawY = y;
		
		// スプライトを描画
		// スプライト画像はすべて右向きに描かれている
		// flip_flag: FALSE = 右向き（反転なし）、TRUE = 左向き（反転あり）
		// DrawRectRotaGraph2の最後のパラメータ: TRUE = 左右反転、FALSE = 反転なし
		// 右向き（flip_flag = FALSE）の時は反転なし（FALSE）、左向き（flip_flag = TRUE）の時は反転（TRUE）
		DrawRectRotaGraph2((int)drawX, (int)drawY, srcX, srcY, spriteWidth, spriteHeight, 
			spriteWidth / 2, spriteHeight / 2, 1.0, 0.0, handle, TRUE, flip_flag);
	}
	else {
		// フォールバック：通常の描画
		if (flip_flag) {
			DrawTurnGraph((int)drawX, (int)y, handle, TRUE);
		}
		else {
			DrawGraph((int)drawX, (int)y, handle, TRUE);
		}
	}
}

void Player::StopAudio() {
	if (footSE != -1 && CheckSoundMem(footSE)) {
		StopSoundMem(footSE);
	}
}
