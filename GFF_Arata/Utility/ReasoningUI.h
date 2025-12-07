#pragma once
#include "DxLib.h"
#include "ReasoningManager.h"
#include <vector>
#include <string>

class ReasoningUI {
private:
    static const int WINDOW_X = 140;
    static const int WINDOW_Y = 60;
    static const int WINDOW_WIDTH = 1000;
    static const int WINDOW_HEIGHT = 600;

    // 色定義
    unsigned int colorBackground;
    unsigned int colorBorder;
    unsigned int colorText;
    unsigned int colorTextSelected;
    unsigned int colorHighlight;
    unsigned int colorLife;

    std::vector<std::string> collectedEvidence;

    void DrawPixelArtBox(int x, int y, int width, int height, unsigned int fill, unsigned int border) const;
    void DrawLife(int current, int max) const;

public:
    ReasoningUI();
    void Initialize();
    void SetEvidenceList(const std::vector<std::string>& evidence);
    void Draw(const ReasoningManager* manager) const;
    void DrawPixelString(int x, int y, unsigned int color, const char* format, ...) const;
};