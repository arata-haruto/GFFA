#include "ChoiceEffect.h"
#include "DxLib.h"
#include <algorithm>
#include <cmath>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

const float ChoiceEffectManager::FLASH_DURATION = 0.3f;
const float ChoiceEffectManager::FADE_DURATION = 0.8f;
const int ChoiceEffectManager::FLASH_ALPHA_MAX = 255;
const int ChoiceEffectManager::FADE_ALPHA_MIN = 80;

void ChoiceEffectManager::UpdateChoiceEffect(ReasoningElement& element, float deltaTime) {
    bool isEnabled = !element.isEliminated;

    if (element.wasEnabled && !isEnabled) {
        element.effectState = ChoiceEffectState::Flash;
        element.flashTimer = 0.0f;
        element.fadeTimer = 0.0f;
        element.alpha = 255.0f;
    }

    element.wasEnabled = isEnabled;

    switch (element.effectState) {
    case ChoiceEffectState::Normal:
        element.alpha = 255.0f;
        break;

    case ChoiceEffectState::Flash:
        element.flashTimer += deltaTime;
        if (element.flashTimer >= FLASH_DURATION) {
            element.effectState = ChoiceEffectState::FadeOut;
            element.fadeTimer = 0.0f;
            element.alpha = 255.0f;
        }
        else {
            float flashProgress = element.flashTimer / FLASH_DURATION;
            element.alpha = 255.0f;
        }
        break;

    case ChoiceEffectState::FadeOut:
        element.fadeTimer += deltaTime;
        if (element.fadeTimer >= FADE_DURATION) {
            element.effectState = ChoiceEffectState::Locked;
            element.alpha = (float)FADE_ALPHA_MIN;
        }
        else {
            float fadeProgress = element.fadeTimer / FADE_DURATION;
            element.alpha = 255.0f - (255.0f - FADE_ALPHA_MIN) * fadeProgress;
        }
        break;

    case ChoiceEffectState::Locked:
        element.alpha = (float)FADE_ALPHA_MIN;
        break;
    }
}

void ChoiceEffectManager::DrawChoice(
    int x, int y, int width, int height,
    const ReasoningElement& element,
    bool isSelected,
    int lockIconHandle
) {
    int alpha = (int)element.alpha;
    alpha = std::max(0, std::min(255, alpha));

    unsigned int bgColor;
    if (element.effectState == ChoiceEffectState::Locked) {
        bgColor = GetColor(35, 35, 35);
    }
    else if (isSelected && element.effectState == ChoiceEffectState::Normal) {
        bgColor = GetColor(120, 100, 80); // 選択中は少し明るく
    }
    else {
        bgColor = GetColor(60, 50, 45);
    }

    // フラッシュ描画
    if (element.effectState == ChoiceEffectState::Flash) {
        float flashProgress = element.flashTimer / FLASH_DURATION;
        int flashIntensity = (int)(255.0f * (1.0f - flashProgress));
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, flashIntensity);
        DrawBox(x, y, x + width, y + height, GetColor(255, 255, 255), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }

    // 背景描画
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
    DrawBox(x, y, x + width, y + height, bgColor, TRUE);

    // 枠線
    if (isSelected && element.effectState == ChoiceEffectState::Normal) {
        DrawBox(x, y, x + width, y + height, GetColor(255, 200, 0), FALSE); // 黄色い枠線
    }
    else if (element.effectState == ChoiceEffectState::Locked) {
        DrawBox(x, y, x + width, y + height, GetColor(100, 100, 100), FALSE);
    }

    unsigned int textColor;
    if (element.effectState == ChoiceEffectState::Locked) {
        textColor = GetColor(100, 100, 100);
    }
    else if (isSelected) {
        textColor = GetColor(255, 255, 0); // 選択文字は黄色
    }
    else {
        textColor = GetColor(255, 255, 255);
    }

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

    // ★カーソル描画の強化
    if (isSelected && element.effectState == ChoiceEffectState::Normal) {
        // 三角形を描画（視認性向上）
        DrawTriangle(x + 5, y + 10, x + 5, y + 26, x + 20, y + 18, GetColor(255, 255, 0), TRUE);
    }

    char buffer[256];
    if (element.effectState == ChoiceEffectState::Locked) {
        sprintf_s(buffer, sizeof(buffer), "%s [除外]", element.text.c_str());
    }
    else {
        sprintf_s(buffer, sizeof(buffer), "%s", element.text.c_str());
    }

    // テキスト位置調整
    DrawFormatString(x + 30, y + 10, textColor, "%s", buffer);

    if (element.effectState == ChoiceEffectState::Locked) {
        int lineY = y + height / 2;
        DrawLine(x + 20, lineY, x + width - 20, lineY, textColor);
        int iconX = x + width - 40;
        int iconY = y + 10;
        DrawLockIcon(iconX, iconY, 20, textColor);
    }

    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void ChoiceEffectManager::DrawLockIcon(int x, int y, int size, unsigned int color) {
    int halfSize = size / 2;
    DrawBox(x + halfSize / 2, y + halfSize, x + halfSize * 3 / 2, y + size, color, FALSE);
    DrawCircle(x + halfSize, y + halfSize, halfSize / 2, color, FALSE);
    DrawCircle(x + halfSize, y + halfSize * 3 / 2, 2, color, TRUE);
}