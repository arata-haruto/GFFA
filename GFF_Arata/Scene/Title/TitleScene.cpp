#include "TitleScene.h"
#include "../../Utility/InputManager.h"
#include "../../Utility/ResourceManager.h"
#include "../../Utility/Vector2D.h"
#include "DxLib.h"

void TitleScene::Initialize()
{
    __super::Initialize();
    ResourceManager* rm = ResourceManager::GetInstance();

    back_ground_image = rm->GetImageResource("Resource/Background/Title.png")[0];
    if (back_ground_image == -1) {
        printfDx("タイトル画像の読み込みに失敗しました\n");
    }

    TitleDoll_image = rm->GetImageResource("Resource/Background/TitleDoll.png")[0];

    select_menu = ePLAY;

    pickup_color = GetColor(255, 0, 0);
    dropoff_color = GetColor(255, 255, 255);

    cursor_sound = -1;
    decision_sound = -1;
}

eSceneType TitleScene::Update(float delta_second)
{
    InputManager* input = InputManager::GetInstance();

    if (input->GetKeyState(KEY_INPUT_DOWN) == eInputState::Pressed ||
        input->GetButtonState(XINPUT_BUTTON_DPAD_DOWN) == eInputState::Pressed)
    {
        SetDownSelectMenuType();
        if (cursor_sound != -1) {
            PlaySoundMem(cursor_sound, DX_PLAYTYPE_BACK);
        }
    }

    if (input->GetKeyState(KEY_INPUT_UP) == eInputState::Pressed ||
        input->GetButtonState(XINPUT_BUTTON_DPAD_UP) == eInputState::Pressed)
    {
        SetUpSelectMenuType();
        if (cursor_sound != -1) {
            PlaySoundMem(cursor_sound, DX_PLAYTYPE_BACK);
        }
    }

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

        case eHELP:
            return eSceneType::eHelp;

        case eEXIT:
            return eSceneType::eExit;

        default:
            return eSceneType::eTitle;
        }
    }

    return GetNowSceneType();
}

void TitleScene::Draw() const
{
    if (back_ground_image != -1) {
        DrawGraph(0, 0, back_ground_image, TRUE);
    }

    if (TitleDoll_image != -1) {
        DrawGraph(550, 0, TitleDoll_image, TRUE);
    }

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
    back_ground_image = -1;
    TitleDoll_image = -1;
    cursor_sound = -1;
    decision_sound = -1;
}

eSceneType TitleScene::GetNowSceneType() const
{
    return eSceneType::eTitle;
}

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
