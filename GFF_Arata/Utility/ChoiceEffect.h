#pragma once
#include "DxLib.h"
#include <string>
#include <set>
#include "ReasoningManager.h" // ReasoningElementの定義が必要

class ChoiceEffectManager {
public:
    static const float FLASH_DURATION;
    static const float FADE_DURATION;
    static const int FLASH_ALPHA_MAX;
    static const int FADE_ALPHA_MIN;

    // エフェクト更新
    static void UpdateChoiceEffect(ReasoningElement& element, float deltaTime);

    // 選択肢描画
    static void DrawChoice(
        int x, int y, int width, int height,
        const ReasoningElement& element,
        bool isSelected,
        int lockIconHandle = -1
    );

    static void DrawLockIcon(int x, int y, int size, unsigned int color);
};