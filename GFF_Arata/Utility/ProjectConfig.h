#pragma once

#include <string>

/**********************************************************
* マクロ定義
***********************************************************/
#define	D_SUCCESS		(0)		// 成功
#define	D_FAILURE		(-1)	// 失敗

#define D_WIN_MAX_X		(1280)	// スクリーンサイズ（幅）
#define D_WIN_MAX_Y		(720)	// スクリーンサイズ（高さ）

#define D_COLOR_BIT		(32)	// カラービット

/**********************************************************
* プロトタイプ宣言
***********************************************************/
/// <summary>
/// フレーム制御
/// </summary>
void FreamControl();

/// <summary>
/// 1フレーム当たりの時間を取得する
/// </summary>
/// <returns>1フレーム当たりの時間</returns>
const float& GetDeltaSecond();

/// <summary>
/// エラー内容を出力する
/// </summary>
/// <returns>終了状態の値</returns>
int ErrorThrow(std::string error_log);