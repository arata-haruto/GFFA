#pragma once
#include "DxLib.h"
#include <string>

// ミニゲームの種類定義
enum class MiniGameType {
    None,
    QuickTimeEvent, // タイミング押し
    PasswordCrack   // 暗号解読
};

class Item {
private:
    static int handle;  // 画像ハンドル
    float x, y;         // 座標
    bool isCollected;   // 取得済みフラグ

    bool showMessage;     // メッセージを表示するフラグ
    float messageTimer;   // メッセージを表示する時間（秒）

    std::string name;        // アイテム名
    std::string description; // アイテム説明文
    MiniGameType gameType;   // ミニゲームの種類

public:
    // コンストラクタ
    Item(float posX, float posY, const std::string& n, const std::string& d,
        MiniGameType type = MiniGameType::None);

    void Init();
    void Update(float playerX, float playerY, float deltaTime);
    void Draw(float cameraOffsetX = 0.0f) const;

    const std::string& GetName() const { return name; }
    const std::string& GetDescription() const { return description; }
    bool GetIsCollected() const { return isCollected; }
    void SetCollected(bool flag) { isCollected = flag; }
    float GetX() const { return x; }
    float GetY() const { return y; }
    MiniGameType GetGameType() const { return gameType; }
};