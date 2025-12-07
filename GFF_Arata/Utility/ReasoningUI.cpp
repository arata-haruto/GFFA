#include "ReasoningUI.h"
#include "ChoiceEffect.h" 
#include "DxLib.h"
#include <cstdarg>
#include <cstdio>

ReasoningUI::ReasoningUI() {
    colorBackground = GetColor(30, 30, 35);
    colorBorder = GetColor(200, 200, 200);
    colorText = GetColor(255, 255, 255);
    colorTextSelected = GetColor(255, 255, 0);
    colorHighlight = GetColor(50, 50, 100);
    colorLife = GetColor(255, 50, 50);
}

void ReasoningUI::Initialize() {
    collectedEvidence.clear();
}

void ReasoningUI::SetEvidenceList(const std::vector<std::string>& evidence) {
    collectedEvidence = evidence;
}

void ReasoningUI::DrawPixelArtBox(int x, int y, int width, int height, unsigned int fill, unsigned int border) const {
    DrawBox(x, y, x + width, y + height, fill, TRUE);
    DrawBox(x, y, x + width, y + height, border, FALSE);
    DrawBox(x + 2, y + 2, x + width - 2, y + height - 2, border, FALSE);
}

void ReasoningUI::DrawPixelString(int x, int y, unsigned int color, const char* format, ...) const {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsprintf_s(buffer, sizeof(buffer), format, args);
    va_end(args);
    DrawFormatString(x, y, color, "%s", buffer);
}

void ReasoningUI::DrawLife(int current, int max) const {
    int startX = WINDOW_X + WINDOW_WIDTH - 200;
    int y = WINDOW_Y - 40;
    DrawPixelString(startX - 60, y, colorText, "Life:");

    for (int i = 0; i < max; i++) {
        if (i < current) {
            DrawBox(startX + i * 40, y, startX + i * 40 + 30, y + 30, colorLife, TRUE);
        }
        else {
            DrawBox(startX + i * 40, y, startX + i * 40 + 30, y + 30, GetColor(100, 100, 100), FALSE);
        }
    }
}

void ReasoningUI::Draw(const ReasoningManager* manager) const {
    if (!manager || !manager->IsActive()) return;

    DrawPixelArtBox(WINDOW_X, WINDOW_Y, WINDOW_WIDTH, WINDOW_HEIGHT, colorBackground, colorBorder);

    // ★変更: 「深夜の雑貨店殺人事件」概要
    DrawPixelString(WINDOW_X + 20, WINDOW_Y + 20, colorText, "【事件概要：深夜の雑貨店殺人事件】");
    DrawPixelString(WINDOW_X + 20, WINDOW_Y + 50, colorText, "被害者：店主（頭部打撲）　現場：ミナト雑貨店");
    DrawPixelString(WINDOW_X + 20, WINDOW_Y + 80, colorText, "容疑者は現場にいた4人。1分以内に真相を暴け！");

    DrawLife(manager->GetCurrentLife(), manager->GetMaxLife());

    DrawPixelString(WINDOW_X + 40, WINDOW_Y + 130, colorTextSelected, "%s", manager->GetStepTitle().c_str());

    DrawPixelString(WINDOW_X + 40, WINDOW_Y + 550, GetColor(200, 200, 255), "現在の推理: %s", manager->GetCurrentSelectionText().c_str());

    std::string hint = manager->GetLastHintMessage();
    if (!hint.empty()) {
        DrawFormatString(WINDOW_X + 40, WINDOW_Y + 500, GetColor(255, 100, 100), "INFO: %s", hint.c_str());
    }

    if (manager->GetCurrentStep() != ReasoningStep::Confirmation) {
        const auto& options = manager->GetCurrentOptions();
        int drawY = WINDOW_Y + 180;
        int idx = 0;

        for (const auto& opt : options) {
            bool isSelected = (idx == manager->GetSelectedIndex());
            ChoiceEffectManager::DrawChoice(
                WINDOW_X + 40, drawY,
                WINDOW_WIDTH - 80, 40,
                opt, isSelected
            );
            drawY += 50;
            idx++;
        }
    }
    else {
        int drawY = WINDOW_Y + 250;
        bool isSelectedYes = (manager->GetSelectedIndex() == 0);

        static ReasoningElement dummyYes("これで推理する！", {}, false);
        static ReasoningElement dummyNo("考え直す...", {}, false);

        ChoiceEffectManager::DrawChoice(WINDOW_X + 200, drawY, 600, 40, dummyYes, isSelectedYes);
        ChoiceEffectManager::DrawChoice(WINDOW_X + 200, drawY + 60, 600, 40, dummyNo, !isSelectedYes);
    }
}