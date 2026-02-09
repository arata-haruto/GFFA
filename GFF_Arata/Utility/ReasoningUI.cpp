#include "ReasoningUI.h"
#include "ChoiceEffect.h" 
#include "DxLib.h"
#include <cstdarg>
#include <cstdio>
#include <vector>
#include <string>
#include <algorithm> 

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif


ReasoningUI::ReasoningUI()
    : colorBackground(0), colorBorder(0),
    colorText(0), colorTextSelected(0),
    colorHighlight(0), colorLife(0)
{
}

void ReasoningUI::Initialize() {
    colorBackground = GetColor(30, 30, 35);      // 濃いグレー
    colorBorder = GetColor(200, 200, 255);     // 薄い青
    colorText = GetColor(255, 255, 255);       // 白
    colorTextSelected = GetColor(255, 255, 0); // 黄色
    colorHighlight = GetColor(0, 100, 200);    // 青
    colorLife = GetColor(255, 50, 50);         // 赤

    collectedEvidence.clear();
}

void ReasoningUI::SetEvidenceList(const std::vector<std::string>& evidence) {
    collectedEvidence = evidence;
}

void ReasoningUI::DrawPixelArtBox(int x, int y, int width, int height, unsigned int fill, unsigned int border) const {
    // 背景塗りつぶし
    DrawBox(x, y, x + width, y + height, fill, TRUE);
    // 枠線
    DrawBox(x, y, x + width, y + height, border, FALSE);
    // 内枠（少し立体感を出す）
    DrawBox(x + 2, y + 2, x + width - 2, y + height - 2, border, FALSE);
}

void ReasoningUI::DrawLife(int current, int max) const {
    int startX = WINDOW_X + WINDOW_WIDTH - 200;
    int y = WINDOW_Y - 40;

    DrawPixelString(startX - 60, y, colorText, "Life:");

    for (int i = 0; i < max; i++) {
        if (i < current) {
            // ハート（塗りつぶし）
            DrawBox(startX + i * 30, y, startX + i * 30 + 20, y + 20, colorLife, TRUE);
        }
        else {
            // ハート（枠のみ）
            DrawBox(startX + i * 30, y, startX + i * 30 + 20, y + 20, GetColor(100, 100, 100), FALSE);
        }
    }
}

void ReasoningUI::DrawPixelString(int x, int y, unsigned int color, const char* format, ...) const {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsprintf_s(buffer, sizeof(buffer), format, args);
    va_end(args);
    DrawFormatString(x, y, color, "%s", buffer);
}

void ReasoningUI::Draw(const ReasoningManager* manager) const {
    if (!manager || !manager->IsActive()) return;

    unsigned int cBg = GetColor(30, 30, 35);
    unsigned int cBorder = GetColor(200, 200, 200);
    unsigned int cText = GetColor(255, 255, 255);
    unsigned int cSel = GetColor(255, 255, 0);

    // メインウィンドウ描画
    DrawPixelArtBox(WINDOW_X, WINDOW_Y, WINDOW_WIDTH, WINDOW_HEIGHT, cBg, cBorder);

    // タイトルと説明
    DrawPixelString(WINDOW_X + 20, WINDOW_Y + 20, cText, "【事件概要：深夜の雑貨店殺人事件】");
    DrawPixelString(WINDOW_X + 20, WINDOW_Y + 50, cText, "被害者：店主（頭部打撲）　現場：ミナト雑貨店");
    DrawPixelString(WINDOW_X + 20, WINDOW_Y + 80, cText, "容疑者は現場にいた4人。1分以内に真相を暴け！");

    // ライフ表示
    DrawLife(manager->GetCurrentLife(), manager->GetMaxLife());

    // 現在のステップタイトル
    DrawPixelString(WINDOW_X + 40, WINDOW_Y + 130, cSel, "%s", manager->GetStepTitle().c_str());

    // 現在の選択状況
    DrawPixelString(WINDOW_X + 40, WINDOW_Y + 550, GetColor(200, 200, 255), "現在の推理: %s", manager->GetCurrentSelectionText().c_str());

    // ヒント表示
    std::string hint = manager->GetLastHintMessage();
    if (!hint.empty()) {
        DrawFormatString(WINDOW_X + 40, WINDOW_Y + 500, GetColor(255, 100, 100), "INFO: %s", hint.c_str());
    }


    if (manager->GetCurrentStep() != ReasoningStep::Confirmation) {
        const auto& options = manager->GetCurrentOptions();
        int drawY = WINDOW_Y + 180; // 描画開始Y座標
        int idx = 0;

        for (const auto& opt : options) {
            bool isSelected = (idx == manager->GetSelectedIndex());

            int boxX = WINDOW_X + 40;
            int boxW = WINDOW_WIDTH - 80;
            int boxH = 40;

            // 背景色
            unsigned int itemBg = GetColor(60, 50, 45);
            if (opt.isEliminated) itemBg = GetColor(35, 35, 35);
            else if (isSelected) itemBg = GetColor(120, 100, 80);

            // 透明度設定
            int alpha = (int)opt.alpha;
            alpha = std::max(0, std::min(255, alpha));
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

            // ボックス描画
            DrawBox(boxX, drawY, boxX + boxW, drawY + boxH, itemBg, TRUE);

            // 枠線
            if (isSelected && !opt.isEliminated) {
                DrawBox(boxX, drawY, boxX + boxW, drawY + boxH, GetColor(255, 200, 0), FALSE);
            }
            else if (opt.isEliminated) {
                DrawBox(boxX, drawY, boxX + boxW, drawY + boxH, GetColor(100, 100, 100), FALSE);
            }

            // テキスト構築
            char buffer[256];
            if (opt.isEliminated) {
                sprintf_s(buffer, sizeof(buffer), "%s [除外]", opt.text.c_str());
            }
            else {
                sprintf_s(buffer, sizeof(buffer), "%s", opt.text.c_str());
            }

            // テキスト色
            unsigned int itemTextCol = cText;
            if (opt.isEliminated) itemTextCol = GetColor(100, 100, 100);
            else if (isSelected) itemTextCol = cSel;

            // カーソル（三角形）
            if (isSelected && !opt.isEliminated) {
                DrawTriangle(boxX + 10, drawY + 10, boxX + 10, drawY + 30, boxX + 25, drawY + 20, cSel, TRUE);
            }

            // 文字描画
            DrawFormatString(boxX + 40, drawY + 10, itemTextCol, "%s", buffer);

            // 打ち消し線（除外時）
            if (opt.isEliminated) {
                DrawLine(boxX + 20, drawY + boxH / 2, boxX + boxW - 20, drawY + boxH / 2, GetColor(100, 100, 100));
            }

            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

            drawY += 50; // 次の項目のY座標へ
            idx++;
        }

        // 操作ガイド
        DrawFormatString(WINDOW_X + 40, WINDOW_Y + 450, GetColor(200, 200, 200),
            "↑↓: 選択  A: 決定  B: 証拠品で除外する");
    }
    else {
        // 確認画面（Yes/No）
        int drawY = WINDOW_Y + 250;
        bool isSelectedYes = (manager->GetSelectedIndex() == 0);

        // Yesボタン
        unsigned int yesColor = isSelectedYes ? cSel : cText;
        DrawBox(WINDOW_X + 200, drawY, WINDOW_X + 800, drawY + 40, isSelectedYes ? GetColor(100, 100, 0) : GetColor(50, 50, 50), TRUE);
        DrawFormatString(WINDOW_X + 450, drawY + 10, yesColor, "これで推理する！");

        // Noボタン
        drawY += 60;
        unsigned int noColor = !isSelectedYes ? cSel : cText;
        DrawBox(WINDOW_X + 200, drawY, WINDOW_X + 800, drawY + 40, !isSelectedYes ? GetColor(100, 100, 0) : GetColor(50, 50, 50), TRUE);
        DrawFormatString(WINDOW_X + 450, drawY + 10, noColor, "考え直す...");
    }
}