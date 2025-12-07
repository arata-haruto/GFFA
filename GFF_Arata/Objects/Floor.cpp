#include "Floor.h"
#include <cmath>

Floor::Floor() {
	x = 0;
	y = 600;      // 画面下部に配置
	width = 1280;
	height = 120;
	handle = -1;
}

void Floor::Initialize() {
	handle = LoadGraph("Resource/Background/Floor.png");
}

void Floor::Draw(float cameraOffsetX) {
	// カメラオフセットを適用して描画
	// マップ全体に地面を繰り返し描画（横スクロール対応）
	const float screenW = 1280.0f;

	// 画面に表示される範囲の地面を描画
	float startX = cameraOffsetX;
	float endX = cameraOffsetX + screenW;

	// ★変更点: 描画ループの範囲をカメラ位置基準にするため、
	// 固定のmapWidth制限ではなく、画面端まで描画し続けるようにループ条件は現状のままでOK
	// ただし、もし背景画像が切れる場合はここを調整する

	// 地面のタイルを繰り返し描画
	float tileStartX = floor(startX / width) * width;
	// 画面外少し余裕を持って描画
	for (float tileX = tileStartX; tileX < endX + width; tileX += width) {
		float drawX = tileX - cameraOffsetX;
		DrawExtendGraph((int)drawX, (int)y, (int)(drawX + width), (int)(y + height), handle, TRUE);
	}
}