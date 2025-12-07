#include "TitleScene.h"
#include "../../Utility/InputManager.h"
#include "../../Utility/ResourceManager.h"
#include "../../Utility/Vector2D.h"
#include "DxLib.h"

void TitleScene::Initialize()
{
	__super::Initialize();

	// 背景画像の読み込み
	back_ground_image = LoadGraph("Resource/Background/Title.png");
	if (back_ground_image == -1) {
		printfDx("タイトル画像の読み込みに失敗しました\n");
	}

	// TitleDoll画像の読み込み（存在する場合）
	TitleDoll_image = LoadGraph("Resource/Background/TitleDoll.png");
	if (TitleDoll_image == -1) {
		// 画像が存在しない場合は-1のまま（描画時にチェック）
	}

	select_menu = ePLAY;

	pickup_color = GetColor(255, 0, 0);
	dropoff_color = GetColor(255, 255, 255);

	// カーソル・決定音の初期化 (コメントアウトされている場合も安全のために初期化)
	cursor_sound = -1;
	decision_sound = -1;

	//cursor_sound = rm->GetSoundResource("Resource/Sound/カーソル移動5.mp3");
	//decision_sound = rm->GetSoundResource("Resource/Sound/決定ボタン押下7.mp3");
}

eSceneType TitleScene::Update(float delta_second)
{
	InputManager* input = InputManager::GetInstance();

	// 下キーが押された場合
	if (input->GetKeyState(KEY_INPUT_DOWN) == eInputState::Pressed ||
		input->GetButtonState(XINPUT_BUTTON_DPAD_DOWN) == eInputState::Pressed)
	{
		SetDownSelectMenuType();
		if (cursor_sound != -1) {
			PlaySoundMem(cursor_sound, DX_PLAYTYPE_BACK);
		}
	}

	// 上キーが押された場合
	if (input->GetKeyState(KEY_INPUT_UP) == eInputState::Pressed ||
		input->GetButtonState(XINPUT_BUTTON_DPAD_UP) == eInputState::Pressed)
	{
		SetUpSelectMenuType();
		if (cursor_sound != -1) {
			PlaySoundMem(cursor_sound, DX_PLAYTYPE_BACK);
		}
	}

	// 決定ボタン
	if (input->GetKeyState(KEY_INPUT_Z) == eInputState::Pressed ||
		input->GetButtonState(XINPUT_BUTTON_A) == eInputState::Pressed)
	{
		if (decision_sound != -1) {
			PlaySoundMem(decision_sound, DX_PLAYTYPE_BACK);
		}
		switch (select_menu)
		{
		case ePLAY:
			return eSceneType::eInGame;
			break;

		case eHELP:
			return eSceneType::eHelp;
			break;

		case eEXIT:
			return eSceneType::eExit;
			break;

		default:
			return eSceneType::eTitle;
			break;
		}
	}

	return GetNowSceneType();
}

void TitleScene::Draw() const
{
	// タイトル背景画像の描画
	if (back_ground_image != -1) {
		DrawGraph(0, 0, back_ground_image, TRUE);
	}

	// TitleDoll画像の描画（存在する場合）
	if (TitleDoll_image != -1) {
		DrawGraph(550, 0, TitleDoll_image, TRUE);
	}

	// タイトルロゴの描画
	DrawFormatString(100, 100, GetColor(255, 255, 255), "タイトル画面");

	// メニューの描画
	if (menu_font_handle != -1) {
		switch (select_menu)
		{
		case ePLAY:
			DrawFormatString(210, 350, pickup_color, "PLAY START");
			DrawFormatString(210, 420, dropoff_color, "HELP");
			DrawFormatString(210, 450, dropoff_color, "EXIT");
			break;
		case eHELP:
			DrawFormatString(210, 350, dropoff_color, "PLAY START");
			DrawFormatString(210, 380, pickup_color, "HELP");
			DrawFormatString(210, 450, dropoff_color, "EXIT");
			break;
		case eEXIT:
			DrawFormatString(210, 350, dropoff_color, "PLAY START");
			DrawFormatString(210, 400, dropoff_color, "HELP");
			DrawFormatString(210, 430, pickup_color, "EXIT");
			break;
		default:
			break;
		}
	}
	else {
		// フォントハンドルが無い場合でもメニューを表示
		switch (select_menu)
		{
		case ePLAY:
			DrawFormatString(210, 350, pickup_color, "PLAY START");
			DrawFormatString(210, 420, dropoff_color, "HELP");
			DrawFormatString(210, 450, dropoff_color, "EXIT");
			break;
		case eHELP:
			DrawFormatString(210, 350, dropoff_color, "PLAY START");
			DrawFormatString(210, 380, pickup_color, "HELP");
			DrawFormatString(210, 450, dropoff_color, "EXIT");
			break;
		case eEXIT:
			DrawFormatString(210, 350, dropoff_color, "PLAY START");
			DrawFormatString(210, 400, dropoff_color, "HELP");
			DrawFormatString(210, 430, pickup_color, "EXIT");
			break;
		default:
			break;
		}
	}
	__super::Draw();
}

void TitleScene::Finalize()
{
	__super::Finalize();

	// 画像の削除
	if (back_ground_image != -1) {
		DeleteGraph(back_ground_image);
		back_ground_image = -1;
	}

	if (TitleDoll_image != -1) {
		DeleteGraph(TitleDoll_image);
		TitleDoll_image = -1;
	}

	// サウンドの削除
	if (cursor_sound != -1) {
		DeleteSoundMem(cursor_sound);
		cursor_sound = -1;
	}
	if (decision_sound != -1) {
		DeleteSoundMem(decision_sound);
		decision_sound = -1;
	}
}

eSceneType TitleScene::GetNowSceneType() const
{
	return eSceneType::eTitle;
}

// 下キー入力時の処理
void TitleScene::SetDownSelectMenuType()
{
	switch (select_menu)
	{
	case eNONE:
		select_menu = ePLAY;
		break;

	case ePLAY:
		select_menu = eHELP;
		break;

	case eHELP:
		select_menu = eEXIT;
		break;

	case eEXIT:
		break;
	default:
		break;
	}
}

// 上キー入力時の処理
void TitleScene::SetUpSelectMenuType()
{
	switch (select_menu)
	{
	case eNONE:
		break;

	case ePLAY:
		break;

	case eHELP:
		select_menu = ePLAY;
		break;

	case eEXIT:
		select_menu = eHELP;
		break;

	default:
		break;
	}
}