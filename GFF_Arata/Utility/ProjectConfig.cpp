#include "ProjectConfig.h"
#include "DxLib.h"

// グローバル変数定義
LONGLONG old_time;		// 前回計測値
LONGLONG now_time;		// 現在計測値
float delta_second;		// １フレームあたりの時間

// フレーム時間計測
void FreamControl()
{
	// 現在時間の取得（μ秒）
	now_time = GetNowHiPerformanceCount();

	// 開始時間から現在時間までに経過した時間を計算する（μ秒）
	// 分解能をμ秒→秒に変換する
	delta_second = (float)(now_time - old_time) * 1.0e-6f;

	// 計測開始時間を更新する
	old_time = now_time;

	// ディスプレイのリフレッシュレートを取得する
	float refresh_rate = (float)GetRefreshRate();

	// １フレーム当たりの時間が1/リフレッシュレート秒を超えたら、整える
	if (delta_second > (1.0f / refresh_rate))
	{
		delta_second = (1.0f / refresh_rate);
	}
}

// 1フレーム当たりの時間を取得する
const float& GetDeltaSecond()
{
	return delta_second;
}

// エラー内容を出力する
int ErrorThrow(std::string error_log)
{
	// Log.txtにエラー内容を追加する
	ErrorLogFmtAdd(error_log.c_str());
	return D_FAILURE;
}