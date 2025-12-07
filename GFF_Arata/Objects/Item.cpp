#include "Item.h"
#include "../Utility/InputManager.h"
#include "DxLib.h"
#include <cmath>

// 静的メンバの定義
int Item::handle = -1;

Item::Item(float posX, float posY, const std::string& n, const std::string& d, MiniGameType type)
    : x(posX), y(posY), name(n), description(d), gameType(type),
    isCollected(false), showMessage(false), messageTimer(0.0f)
{
}

void Item::Init() {
    if (handle == -1) {
        handle = LoadGraph("Resource/Objects/Evidence.png");
        if (handle == -1) {
            printfDx("証拠画像(Evidence.png)が見つかりません - プレースホルダーで描画します\n");
        }
    }
}

void Item::Update(float playerX, float playerY, float deltaTime) {
    if (isCollected) return;

    // プレイヤーとの距離計算
    float dx = playerX - x;
    float dy = playerY - y;
    float distance = sqrt(dx * dx + dy * dy);

    const float interactionRange = 80.0f;
    bool isNearby = (distance < interactionRange);

    if (isNearby) {
        InputManager* input = InputManager::GetInstance();

        if (input->GetKeyState(KEY_INPUT_Z) == eInputState::Pressed ||
            input->GetButtonState(XINPUT_BUTTON_A) == eInputState::Pressed) {

            isCollected = true;
            showMessage = true;
            messageTimer = 2.0f;
            printfDx("証拠を入手: %s\n", name.c_str());
        }
    }

    if (showMessage) {
        messageTimer -= deltaTime;
        if (messageTimer <= 0.0f) {
            showMessage = false;
        }
    }
}

void Item::Draw(float cameraOffsetX) const {
    if (isCollected) return;

    float drawX = x - cameraOffsetX;

    // 画面外なら描画しない
    if (drawX < -100 || drawX > 1380) return;

    // 光るエフェクト
    static float animTime = 0.0f;
    animTime += 0.05f;
    if (animTime > 6.28f) animTime = 0.0f;

    int glowAlpha = (int)(128 + 127 * sin(animTime));

    // 外側の光
    SetDrawBlendMode(DX_BLENDMODE_ADD, glowAlpha / 2);
    DrawCircle((int)drawX, (int)y, 35, GetColor(255, 255, 0), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    // 内側の光
    SetDrawBlendMode(DX_BLENDMODE_ADD, glowAlpha);
    DrawCircle((int)drawX, (int)y, 20, GetColor(255, 215, 0), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    // アイテム本体
    if (handle != -1) {
        DrawGraph((int)drawX - 16, (int)y - 16, handle, TRUE);
    }
    else {
        DrawCircle((int)drawX, (int)y, 16, GetColor(255, 200, 0), TRUE);
        DrawCircle((int)drawX, (int)y, 16, GetColor(255, 255, 255), FALSE);
        DrawFormatString((int)drawX - 4, (int)y - 8, GetColor(0, 0, 0), "!");
    }

    // アイテム名表示
    int nameX = (int)drawX - GetDrawStringWidth(name.c_str(), -1) / 2;
    int nameY = (int)y - 50;

    int textWidth = GetDrawStringWidth(name.c_str(), -1);
    DrawBox(nameX - 5, nameY - 2, nameX + textWidth + 5, nameY + 18,
        GetColor(0, 0, 0), TRUE);

    DrawFormatString(nameX, nameY, GetColor(255, 255, 100), "%s", name.c_str());

    // ミニゲームアイコン
    if (gameType != MiniGameType::None) {
        int iconX = (int)drawX + 20;
        int iconY = (int)y - 20;

        DrawCircle(iconX, iconY, 12, GetColor(255, 100, 100), TRUE);
        DrawCircle(iconX, iconY, 12, GetColor(255, 255, 255), FALSE);

        const char* iconText = "";
        switch (gameType) {
        case MiniGameType::QuickTimeEvent:
            iconText = "Q";
            break;
        case MiniGameType::PasswordCrack:
            iconText = "P";
            break;
        default:
            break;
        }

        if (iconText[0] != '\0') {
            int textW = GetDrawStringWidth(iconText, -1);
            DrawFormatString(iconX - textW / 2, iconY - 6, GetColor(255, 255, 255), "%s", iconText);
        }
    }

    // 取得メッセージ
    if (showMessage) {
        int msgX = 640 - 120;
        int msgY = 100;
        int msgW = 240;
        int msgH = 80;

        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 230);
        DrawBox(msgX, msgY, msgX + msgW, msgY + msgH, GetColor(20, 20, 40), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

        DrawBox(msgX, msgY, msgX + msgW, msgY + msgH, GetColor(255, 255, 100), FALSE);
        DrawBox(msgX + 2, msgY + 2, msgX + msgW - 2, msgY + msgH - 2, GetColor(200, 200, 80), FALSE);

        DrawFormatString(msgX + 10, msgY + 10, GetColor(255, 255, 255), "証拠を入手！");

        std::string displayName = name;
        if (displayName.length() > 20) {
            displayName = displayName.substr(0, 20) + "...";
        }
        DrawFormatString(msgX + 10, msgY + 35, GetColor(200, 200, 200), "%s", displayName.c_str());

        std::string displayDesc = description;
        if (displayDesc.length() > 20) {
            displayDesc = displayDesc.substr(0, 20) + "...";
        }
        DrawFormatString(msgX + 10, msgY + 55, GetColor(150, 150, 150), "%s", displayDesc.c_str());
    }
}
