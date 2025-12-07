#pragma once
#include "DxLib.h"
#include <string>
#include <vector>

// 証言データ構造体
struct Testimony {
    std::string text;            // 証言内容
    bool isImportant;            // 重要な証言か
    bool requiresEvidence;       // 特定の証拠が必要か
    std::string requiredEvidence; // 必要な証拠名
};

// NPC状態
enum class NPCState {
    Idle,            // 待機
    Talking,         // 会話中
    Suspicious,      // 怪しい反応
    Cooperative      // 協力的
};

// NPCクラス
class NPC {
protected:
    float x, y;                     // 座標
    int handle;                     // 画像ハンドル
    std::string name;               // 名前
    std::string role;               // 役割（容疑者、目撃者など）
    NPCState state;                 // 状態

    std::vector<Testimony> testimonies; // 証言リスト
    int currentTestimonyIndex;      // 現在の証言インデックス

    bool isInteracting;             // プレイヤーが近づいているか
    bool hasBeenQuestioned;         // 既に質問されたか

    // エフェクト用
    float animTimer;                // アニメーションタイマー
    bool flip;                      // 向き（左右反転）

public:
    NPC(float posX, float posY, const std::string& n, const std::string& r);
    virtual ~NPC();

    virtual void Initialize();
    virtual void Update(float playerX, float playerY, float deltaTime);
    virtual void Draw(float cameraOffsetX = 0.0f) const;

    // 証言関連
    void AddTestimony(const Testimony& testimony);
    const Testimony* GetCurrentTestimony() const;
    const std::vector<Testimony>& GetAllTestimonies() const { return testimonies; }
    bool HasMoreTestimonies() const;
    void NextTestimony();
    void ResetTestimonies();

    // ゲッター
    const std::string& GetName() const { return name; }
    const std::string& GetRole() const { return role; }
    bool IsInteracting() const { return isInteracting; }
    bool HasBeenQuestioned() const { return hasBeenQuestioned; }
    NPCState GetState() const { return state; }
    float GetX() const { return x; }
    float GetY() const { return y; }

    // セッター
    void SetState(NPCState newState) { state = newState; }
    void SetQuestioned(bool questioned) { hasBeenQuestioned = questioned; }
};