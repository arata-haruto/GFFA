#pragma once
#include "../Objects/NPC.h"
#include "../Objects/Suspect.h"
#include <vector>
#include <memory>

class NPCManager {
private:
    std::vector<std::unique_ptr<NPC>> npcs;
    NPC* currentTalkingNPC;             // 現在会話中のNPC
    bool isDialogueActive;              // ダイアログ表示中か

    // ダイアログUI
    int dialogueBoxHandle;
    float dialogueTimer;

public:
    NPCManager();
    ~NPCManager();

    void Initialize();
    void Update(float playerX, float playerY, float deltaTime);
    void Draw(float cameraOffsetX = 0.0f) const;

    // NPC追加
    void AddSuspect(SuspectType type, float x, float y);
    void AddNPC(NPC* npc);

    // 会話システム
    void StartDialogue(NPC* npc);
    void EndDialogue();
    void NextDialogue();
    bool IsDialogueActive() const { return isDialogueActive; }

    // ダイアログ描画
    void DrawDialogue() const;

    // ゲッター
    const std::vector<std::unique_ptr<NPC>>& GetAllNPCs() const { return npcs; }
};