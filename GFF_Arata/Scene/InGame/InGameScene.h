#pragma once
#include "../SceneBase.h"
#include "../../Objects/Player.h"
#include "../../Objects/NPC.h"
#include "../../Utility/NPCManager.h"
#include "../../Utility/DialogueSystem.h"
#include "../../Utility/ReasoningManager.h"
#include "../../Utility/ReasoningUI.h"
#include "../../Utility/ItemManager.h"
#include "MapData.h"
#include <vector>

enum class GamePhase {
    Opening,
    EvidenceCollection,
    NPCDialogue,
    Reasoning,
    Result,
    MiniGame,
    AreaTransition
};

struct RuntimeBgObject {
    int handle;
    float x, y;
    float width, height;
};

class InGameScene : public SceneBase {
private:
    Player* player1;
    ItemManager itemManager;
    NPCManager npcManager;
    DialogueSystem* dialogueSystem;

    int charHandle_Player;
    int charHandle_Police;
    int charHandle_Fujisaki;
    int charHandle_Sasaki;
    int charHandle_Kimura;
    int charHandle_Yamada;

    std::vector<RuntimeBgObject> backgroundObjects;
    int bgHandles[5];
    int currentAreaIndex;
    int nextAreaIndex;
    float fadeAlpha;
    bool isFadingOut;

    float timeLimit;
    float remainingTime;
    bool timerPaused;
    bool allEvidenceCollected;

    GamePhase currentPhase;
    bool isSelectingEvidence;
    float evidenceResultTimer;
    std::string evidenceResultMessage;

    float mg_barPosition;
    float mg_barSpeed;
    float mg_targetMin;
    float mg_targetMax;
    Item* mg_targetItem;
    float mg_resultTimer;
    bool mg_lastResultSuccess;
    Item* currentInteractingItem;

    ReasoningManager* reasoningManager;
    ReasoningUI* reasoningUI;

    bool showResult;
    bool isCorrect;
    bool isGameOver;
    float resultDisplayTime;

    mutable float cameraX;

    int mainbgm;
    int back_ground_image;
    int se_success;
    int se_fail;

    bool showNPCIndicator;

public:
    InGameScene();
    virtual ~InGameScene();

    void Initialize() override;
    eSceneType Update(float delta_second) override;
    void Draw() const override;
    void Finalize() override;
    eSceneType GetNowSceneType() const override;

private:
    void LoadCharacterImages();
    void SetupOpeningDialogue();
    void SetupNPCDialogue(NPC* npc);

    void TransitionToReasoning();
    void DrawTimer() const;
    void DrawPhaseInfo() const;
    void DrawResult() const;
    void DrawOpening() const;
    void DrawAreaInfo() const;

    void DrawNPCIndicator() const;
    void DrawBackgroundObjects(float cameraOffsetX) const;
    void UpdateBackground();
    void LoadBackgroundObjects();
    int CountQuestionedNPCs() const;

    void CheckAreaTransition();
    void UpdateAreaTransition(float delta_second);
    void ChangeArea(int newAreaIndex);

    void StartMiniGame(Item* item);
    void OnMiniGameComplete(bool success, Item* item);
    void UpdateMiniGame(float delta_second);
    void DrawMiniGame() const;
};