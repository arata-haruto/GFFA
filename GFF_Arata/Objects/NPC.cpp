#include "NPC.h"
#include "../Utility/InputManager.h"
#include <cmath>

NPC::NPC(float posX, float posY, const std::string& n, const std::string& r)
    : x(posX), y(posY), name(n), role(r),
    handle(-1), state(NPCState::Idle),
    currentTestimonyIndex(0), isInteracting(false),
    hasBeenQuestioned(false), animTimer(0.0f), flip(false)
{
}

NPC::~NPC() {
    if (handle != -1) {
        DeleteGraph(handle);
    }
}

void NPC::Initialize() {
    // デフォルト画像読み込み（プレースホルダー）
    // handle = LoadGraph("Resource/Characters/NPC/Default.png");
}

void NPC::Update(float playerX, float playerY, float deltaTime) {
    // アニメーションタイマー更新
    animTimer += deltaTime * 2.0f;
    if (animTimer > 6.28f) animTimer = 0.0f;

    // プレイヤーとの距離判定
    float dx = playerX - x;
    float dy = playerY - y;
    float distance = sqrt(dx * dx + dy * dy);

    const float interactionRange = 100.0f;
    isInteracting = (distance < interactionRange);

    // プレイヤーの方向を向く
    if (isInteracting) {
        flip = (dx < 0);
    }

    // 状態に応じた挙動
    switch (state) {
    case NPCState::Suspicious:
        // 怪しい反応（少し後ずさりするような演出など）
        break;
    }
}

void NPC::Draw(float cameraOffsetX) const {
    float drawX = x - cameraOffsetX;

    // 画面外なら描画しない
    if (drawX < -100 || drawX > 1380) return;

    // NPC本体描画
    if (handle != -1) {
        if (flip) {
            DrawTurnGraph((int)drawX - 25, (int)y - 150, handle, TRUE);
        }
        else {
            DrawGraph((int)drawX - 25, (int)y - 150, handle, TRUE);
        }
    }
    else {
        // プレースホルダー（四角）
        unsigned int color;
        switch (state) {
        case NPCState::Suspicious: color = GetColor(255, 100, 100); break; // 赤
        case NPCState::Cooperative: color = GetColor(100, 255, 100); break; // 緑
        case NPCState::Talking: color = GetColor(100, 100, 255); break; // 青
        default: color = GetColor(200, 200, 200); break; // グレー
        }
        DrawBox((int)drawX - 20, (int)y - 160, (int)drawX + 20, (int)y, color, TRUE);
        DrawBox((int)drawX - 20, (int)y - 160, (int)drawX + 20, (int)y, GetColor(255, 255, 255), FALSE);
    }

    // 名前表示
    int nameWidth = GetDrawStringWidth(name.c_str(), -1);
    int nameX = (int)drawX - nameWidth / 2;
    DrawFormatString(nameX, (int)y - 180, GetColor(255, 255, 255), "%s", name.c_str());

    // 役割表示
    int roleWidth = GetDrawStringWidth(role.c_str(), -1);
    int roleX = (int)drawX - roleWidth / 2;
    DrawFormatString(roleX, (int)y - 195, GetColor(200, 200, 100), "<%s>", role.c_str());

    // インタラクション可能表示
    if (isInteracting) {
        // 吹き出しアイコン
        int iconY = (int)y - 210 + (int)(5 * sin(animTimer * 3));
        DrawCircle((int)drawX, iconY, 10, GetColor(255, 255, 255), FALSE, 2);
        DrawFormatString((int)drawX + 15, iconY - 5, GetColor(255, 255, 0), "[Z] 話す");

        // 既に質問済みの場合
        if (hasBeenQuestioned) {
            DrawFormatString((int)drawX - 40, (int)y + 10, GetColor(150, 150, 150), "(質問済み)");
        }
    }

    // 状態インジケーター
    const char* stateText = nullptr;
    unsigned int stateColor = GetColor(255, 255, 255);

    switch (state) {
    case NPCState::Suspicious:
        stateText = "!";
        stateColor = GetColor(255, 50, 50);
        break;
    case NPCState::Cooperative:
        stateText = "♪";
        stateColor = GetColor(100, 255, 100);
        break;
    case NPCState::Talking:
        stateText = "...";
        stateColor = GetColor(100, 200, 255);
        break;
    }

    if (stateText) {
        DrawFormatString((int)drawX + 25, (int)y - 150, stateColor, "%s", stateText);
    }
}

void NPC::AddTestimony(const Testimony& testimony) {
    testimonies.push_back(testimony);
}

const Testimony* NPC::GetCurrentTestimony() const {
    if (currentTestimonyIndex < (int)testimonies.size()) {
        return &testimonies[currentTestimonyIndex];
    }
    return nullptr;
}

bool NPC::HasMoreTestimonies() const {
    return currentTestimonyIndex < (int)testimonies.size() - 1;
}

void NPC::NextTestimony() {
    if (HasMoreTestimonies()) {
        currentTestimonyIndex++;
    }
}

void NPC::ResetTestimonies() {
    currentTestimonyIndex = 0;
    hasBeenQuestioned = false;
}