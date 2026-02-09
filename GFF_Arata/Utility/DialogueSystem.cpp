#include "DialogueSystem.h"
#include "InputManager.h"
#include <algorithm>
#include <cmath>

DialogueSystem::DialogueSystem()
    : currentIndex(0), isActive(false), isFinished(false),
    displayCharCount(0), textSpeed(0.05f), textTimer(0.0f),
    isTextComplete(false), arrowAnimTimer(0.0f)
{
    // UI座標設定
    textBoxX = 50;
    textBoxY = 480;        // 下部に配置
    textBoxWidth = 1180;
    textBoxHeight = 200;
    nameBoxWidth = 300;
    nameBoxHeight = 50;

    // 色設定
    colorTextBox = GetColor(0, 0, 0);           // 黒背景
    colorNameBox = GetColor(40, 40, 60);        // 名前枠
    colorBorder = GetColor(255, 255, 255);      // 白枠
    colorText = GetColor(255, 255, 255);        // 白文字
    colorNameText = GetColor(255, 255, 100);    // 黄色い名前
    colorArrow = GetColor(255, 100, 100);       // 赤い▼マーク
}

DialogueSystem::~DialogueSystem() {
    Finalize();
}

void DialogueSystem::Initialize() {
    dialogues.clear();
    currentIndex = 0;
    isActive = false;
    isFinished = false;
    displayedText = "";
    displayCharCount = 0;
}

void DialogueSystem::Finalize() {
    dialogues.clear();
}

void DialogueSystem::AddDialogue(const DialogueData& data) {
    dialogues.push_back(data);
}

void DialogueSystem::SetDialogues(const std::vector<DialogueData>& dialogueList) {
    dialogues = dialogueList;
}

void DialogueSystem::ClearDialogues() {
    dialogues.clear();
    currentIndex = 0;
}

void DialogueSystem::Update(float deltaTime) {
    if (!isActive) return;

    // アニメーション更新
    arrowAnimTimer += deltaTime * 5.0f;

    // テキスト表示更新
    if (!isTextComplete) {
        UpdateTextDisplay(deltaTime);
    }

    InputManager* input = InputManager::GetInstance();

    // 入力処理
    if (input->GetKeyState(KEY_INPUT_Z) == eInputState::Pressed ||
        input->GetButtonState(XINPUT_BUTTON_A) == eInputState::Pressed) {
        
        if (!isTextComplete) {
            SkipText(); // テキストを最後まで表示
        }
        else {
            NextDialogue(); // 次の会話へ
        }
    }
}

void DialogueSystem::UpdateTextDisplay(float deltaTime) {
    textTimer += deltaTime;

    while (textTimer >= textSpeed) {
        if (displayCharCount < dialogues[currentIndex].text.length()) {
            // マルチバイト文字判定（簡易版）
            unsigned char c = (unsigned char)dialogues[currentIndex].text[displayCharCount];
            int charLen = 1;
            
            if ((c & 0x80) == 0) charLen = 1;
            else if ((c & 0xE0) == 0xC0) charLen = 2;
            else if ((c & 0xF0) == 0xE0) charLen = 3; // 日本語など
            else if ((c & 0xF8) == 0xF0) charLen = 4;

            displayCharCount += charLen;
            textTimer -= textSpeed;
        }
        else {
            isTextComplete = true;
            displayCharCount = (int)dialogues[currentIndex].text.length();
            break;
        }
    }

    displayedText = dialogues[currentIndex].text.substr(0, displayCharCount);
}

void DialogueSystem::Draw() const {
    if (!isActive) return;

    // 1. キャラクター立ち絵
    DrawCharacters();

    // 2. テキストボックス
    DrawTextBox();

    // 3. 名前枠と名前
    DrawNameBox();

    // 4. テキスト本文
    DrawText();

    // 5. ▼マーク
    if (isTextComplete) {
        DrawProgressArrow();
    }
}

void DialogueSystem::DrawCharacters() const {
    if (currentIndex >= dialogues.size()) return;

    const auto& data = dialogues[currentIndex];

    // 左側のキャラ
    if (data.leftCharacterHandle != -1) {
        int w, h;
        GetGraphSize(data.leftCharacterHandle, &w, &h);
        
        // 話していない時は少し暗くする
        if (data.speakerPos != CharacterPosition::Left) {
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
            SetDrawBright(150, 150, 150);
        } else {
            SetDrawBright(255, 255, 255);
        }
        
        // 立ち絵の位置調整（左）
        // DrawGraph(200 - w/2, 100, data.leftCharacterHandle, TRUE);
        // 位置調整: 画像の下端を合わせる
        DrawGraph(150, 720 - h - 200, data.leftCharacterHandle, TRUE);
        
        SetDrawBright(255, 255, 255); // 戻す
    }

    // 右側のキャラ
    if (data.rightCharacterHandle != -1) {
        int w, h;
        GetGraphSize(data.rightCharacterHandle, &w, &h);

        // 話していない時は少し暗くする
        if (data.speakerPos != CharacterPosition::Right) {
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
            SetDrawBright(150, 150, 150);
        } else {
            SetDrawBright(255, 255, 255);
        }

        // 立ち絵の位置調整（右）
        DrawGraph(1280 - 150 - w, 720 - h - 200, data.rightCharacterHandle, TRUE);

        SetDrawBright(255, 255, 255); // 戻す
    }
}

void DialogueSystem::DrawTextBox() const {
    // 半透明の背景
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
    DrawBox(textBoxX, textBoxY, textBoxX + textBoxWidth, textBoxY + textBoxHeight, colorTextBox, TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    // 枠線
    DrawBox(textBoxX, textBoxY, textBoxX + textBoxWidth, textBoxY + textBoxHeight, colorBorder, FALSE);
    // 内枠
    DrawBox(textBoxX + 4, textBoxY + 4, textBoxX + textBoxWidth - 4, textBoxY + textBoxHeight - 4, GetColor(150, 150, 150), FALSE);
}

void DialogueSystem::DrawNameBox() const {
    if (currentIndex >= dialogues.size() || dialogues[currentIndex].speakerName.empty()) return;

    int nbX = textBoxX + 20;
    int nbY = textBoxY - 30;

    // 名前枠背景
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
    DrawBox(nbX, nbY, nbX + nameBoxWidth, nbY + nameBoxHeight, colorNameBox, TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    
    // 名前枠線
    DrawBox(nbX, nbY, nbX + nameBoxWidth, nbY + nameBoxHeight, colorBorder, FALSE);

    // 名前テキスト
    SetFontSize(24);
    DrawFormatString(nbX + 20, nbY + 13, colorNameText, "%s", dialogues[currentIndex].speakerName.c_str());
    SetFontSize(16); // 戻す
}

void DialogueSystem::DrawText() const {
    if (displayedText.empty()) return;

    int startX = textBoxX + 40;
    int startY = textBoxY + 40;
    int lineHeight = 30;

    SetFontSize(24);

    // 自動改行処理を行いながら描画
    std::vector<std::string> lines = WrapText(displayedText, textBoxWidth - 80);
    
    for (size_t i = 0; i < lines.size(); ++i) {
        DrawString(startX, startY + (int)i * lineHeight, lines[i].c_str(), colorText);
    }

    SetFontSize(16); // 戻す
}

void DialogueSystem::DrawProgressArrow() const {
    int x = textBoxX + textBoxWidth - 40;
    int y = textBoxY + textBoxHeight - 40 + (int)(sin(arrowAnimTimer) * 5.0f);

    // 逆三角形を描画
    DrawTriangle(x, y, x + 20, y, x + 10, y + 10, colorArrow, TRUE);
}

void DialogueSystem::Start() {
    if (dialogues.empty()) return;
    isActive = true;
    isFinished = false;
    currentIndex = 0;
    displayCharCount = 0;
    textTimer = 0.0f;
    isTextComplete = false;
    displayedText = "";
}

void DialogueSystem::NextDialogue() {
    if (currentIndex < dialogues.size() - 1) {
        currentIndex++;
        displayCharCount = 0;
        textTimer = 0.0f;
        isTextComplete = false;
        displayedText = "";
    } else {
        // 会話終了
        Stop();
    }
}

void DialogueSystem::SkipText() {
    if (currentIndex < dialogues.size()) {
        displayedText = dialogues[currentIndex].text;
        displayCharCount = (int)displayedText.length();
        isTextComplete = true;
    }
}

void DialogueSystem::Stop() {
    isActive = false;
    isFinished = true;
}

std::vector<std::string> DialogueSystem::WrapText(const std::string& text, int maxWidth) const {
    std::vector<std::string> lines;
    std::string currentLine;

    for (size_t i = 0; i < text.length(); ) {
        // 1文字取得（マルチバイト対応）
        unsigned char c = (unsigned char)text[i];
        int byteCount = 1;

        if ((c & 0x80) == 0x00) {
            byteCount = 1;  // ASCII
        }
        else if ((c & 0xE0) == 0xC0) {
            byteCount = 2;
        }
        else if ((c & 0xF0) == 0xE0) {
            byteCount = 3;  // 日本語
        }
        else if ((c & 0xF8) == 0xF0) {
            byteCount = 4;
        }

        std::string character = text.substr(i, byteCount);
        std::string testLine = currentLine + character;

        // 改行コード検出
        if (character == "\n") {
            lines.push_back(currentLine);
            currentLine.clear();
            i += byteCount;
            continue;
        }

        // 幅チェック
        if (GetDrawStringWidth(testLine.c_str(), -1) > maxWidth && !currentLine.empty()) {
            lines.push_back(currentLine);
            currentLine = character;
        }
        else {
            currentLine = testLine;
        }

        i += byteCount;
    }

    if (!currentLine.empty()) {
        lines.push_back(currentLine);
    }

    return lines;
}