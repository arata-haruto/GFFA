#pragma once
#include "DxLib.h"
#include <string>
#include <vector>

// 立ち絵の位置
enum class CharacterPosition {
    Left,      // 左側
    Right,     // 右側
    Center,    // 中央
    None       // 非表示
};

// 会話データ
struct DialogueData {
    std::string speakerName;           // 話者名
    std::string text;                  // セリフ
    CharacterPosition speakerPos;      // 話者の立ち絵位置
    int leftCharacterHandle;           // 左側のキャラクター画像ハンドル
    int rightCharacterHandle;          // 右側のキャラクター画像ハンドル
    bool waitForInput;                 // 入力待ちするか

    DialogueData(const std::string& name, const std::string& msg,
        CharacterPosition pos = CharacterPosition::Left,
        int leftChar = -1, int rightChar = -1, bool wait = true)
        : speakerName(name), text(msg), speakerPos(pos),
        leftCharacterHandle(leftChar), rightCharacterHandle(rightChar),
        waitForInput(wait)
    {}
};

class DialogueSystem {
private:
    std::vector<DialogueData> dialogues;  // 会話データリスト
    int currentIndex;                      // 現在の会話インデックス
    bool isActive;                         // 会話システムが有効か
    bool isFinished;                       // 会話が終了したか

    // UI要素
    int textBoxX, textBoxY;
    int textBoxWidth, textBoxHeight;
    int nameBoxWidth, nameBoxHeight;

    // テキスト表示関連
    std::string displayedText;             // 現在表示中のテキスト
    int displayCharCount;                  // 表示文字数
    float textSpeed;                       // テキスト表示速度
    float textTimer;                       // タイマー
    bool isTextComplete;                   // テキスト表示完了フラグ
    float arrowAnimTimer;                  // ▼マークのアニメーションタイマー

    // 色定義
    unsigned int colorTextBox;
    unsigned int colorNameBox;
    unsigned int colorBorder;
    unsigned int colorText;
    unsigned int colorNameText;
    unsigned int colorArrow;

public:
    DialogueSystem();
    ~DialogueSystem();

    // 初期化・終了
    void Initialize();
    void Finalize();

    // 会話データの追加・設定
    void AddDialogue(const DialogueData& data);
    void SetDialogues(const std::vector<DialogueData>& dialogueList);
    void ClearDialogues();

    // 更新・描画
    void Update(float deltaTime);
    void Draw() const;

    // 会話制御
    void Start();                          // 会話開始
    void NextDialogue();                   // 次の会話へ
    void SkipText();                       // テキスト表示を即座に完了
    void Stop();                           // 会話停止

    // 状態取得
    bool IsActive() const { return isActive; }
    bool IsFinished() const { return isFinished; }
    bool IsTextComplete() const { return isTextComplete; }
    int GetCurrentIndex() const { return currentIndex; }
    int GetTotalCount() const { return (int)dialogues.size(); }

private:
    // 内部処理
    void UpdateTextDisplay(float deltaTime);
    void DrawTextBox() const;
    void DrawNameBox() const;
    void DrawCharacters() const;
    void DrawText() const;
    void DrawProgressArrow() const;

    // テキスト処理
    std::vector<std::string> WrapText(const std::string& text, int maxWidth) const;
};