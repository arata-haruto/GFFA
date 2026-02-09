#include "HelpScene.h"
#include "../../Utility/InputManager.h"
#include "DxLib.h"

HelpScene::HelpScene()
	: back_ground_image(-1),
	title_font_handle(-1),
	menu_font_handle(-1)
{
}

HelpScene::~HelpScene()
{
	Finalize();
}

void HelpScene::Initialize()
{
	__super::Initialize();

	
}

eSceneType HelpScene::Update(float delta_second)
{
	InputManager* input = InputManager::GetInstance();

	// ZキーまたはAボタンでタイトルに戻る
	if (input->GetKeyState(KEY_INPUT_Z) == eInputState::Pressed ||
		input->GetKeyState(KEY_INPUT_X) == eInputState::Pressed ||
		input->GetKeyState(KEY_INPUT_ESCAPE) == eInputState::Pressed ||
		input->GetButtonState(XINPUT_BUTTON_A) == eInputState::Pressed ||
		input->GetButtonState(XINPUT_BUTTON_B) == eInputState::Pressed)
	{
		return eSceneType::eTitle;
	}

	return GetNowSceneType();
}

void HelpScene::Draw() const
{
	// 背景画像の描画
	if (back_ground_image != -1) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
		DrawGraph(0, 0, back_ground_image, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	else {
		// 背景画像がない場合は黒背景
		DrawBox(0, 0, 1280, 720, GetColor(0, 0, 0), TRUE);
	}

	// タイトル
	SetFontSize(48);
	DrawFormatString(400, 50, GetColor(255, 255, 0), "操作方法");

	SetFontSize(24);

	// キーボード操作
	int startY = 120;
	int lineHeight = 35;
	DrawFormatString(100, startY, GetColor(255, 200, 100), "【キーボード操作】");
	
	startY += lineHeight + 10;
	DrawFormatString(120, startY, GetColor(255, 255, 255), "移動: ← → キー");
	startY += lineHeight;
	DrawFormatString(120, startY, GetColor(255, 255, 255), "会話・決定: Z キー");
	startY += lineHeight;
	DrawFormatString(120, startY, GetColor(255, 255, 255), "証拠品リスト: E キー");
	startY += lineHeight;
	DrawFormatString(120, startY, GetColor(255, 255, 255), "容疑者位置表示: TAB キー");
	startY += lineHeight;
	DrawFormatString(120, startY, GetColor(255, 255, 255), "証拠で除外: X キー（推理時）");
	startY += lineHeight;
	DrawFormatString(120, startY, GetColor(255, 255, 255), "タイトルに戻る: A キー");
	startY += lineHeight;
	DrawFormatString(120, startY, GetColor(255, 255, 255), "キャンセル: X キー");

	// コントローラー操作
	startY += lineHeight + 20;
	DrawFormatString(100, startY, GetColor(255, 200, 100), "【コントローラー操作】");
	
	startY += lineHeight + 10;
	DrawFormatString(120, startY, GetColor(255, 255, 255), "移動: 左スティック / 十字キー");
	startY += lineHeight;
	DrawFormatString(120, startY, GetColor(255, 255, 255), "会話・決定: A ボタン");
	startY += lineHeight;
	DrawFormatString(120, startY, GetColor(255, 255, 255), "証拠品リスト: Y ボタン");
	startY += lineHeight;
	DrawFormatString(120, startY, GetColor(255, 255, 255), "容疑者位置表示: BACK ボタン");
	startY += lineHeight;
	DrawFormatString(120, startY, GetColor(255, 255, 255), "証拠で除外: Y ボタン（推理時）");
	startY += lineHeight;
	DrawFormatString(120, startY, GetColor(255, 255, 255), "タイトルに戻る: START ボタン");
	startY += lineHeight;
	DrawFormatString(120, startY, GetColor(255, 255, 255), "キャンセル: B ボタン");

	// ゲームの説明
	startY += lineHeight + 30;
	DrawFormatString(100, startY, GetColor(255, 200, 100), "【ゲームの目的】");
	startY += lineHeight + 10;
	DrawFormatString(120, startY, GetColor(255, 255, 255), "60秒以内に証拠を集め、容疑者から話を聞き、");
	startY += lineHeight;
	DrawFormatString(120, startY, GetColor(255, 255, 255), "犯人・動機・凶器を特定してください。");
	startY += lineHeight;
	DrawFormatString(120, startY, GetColor(255, 255, 255), "集めた証拠で矛盾を暴き、選択肢を除外できます。");

	// 戻る方法の表示
	SetFontSize(20);
	DrawFormatString(500, 680, GetColor(200, 200, 200), "Z キー / A ボタン / B ボタン / ESC キーでタイトルに戻る");

	__super::Draw();
}

void HelpScene::Finalize()
{
	__super::Finalize();

	// 画像の削除
	if (back_ground_image != -1) {
		DeleteGraph(back_ground_image);
		back_ground_image = -1;
	}
}

eSceneType HelpScene::GetNowSceneType() const
{
	return eSceneType::eHelp;
}

