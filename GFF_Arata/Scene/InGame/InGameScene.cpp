#include "InGameScene.h"
#include "../SceneFactory.h"
#include "../../Utility/InputManager.h"
#include "../../Utility/ResourceManager.h"
#include "../../Utility/Vector2D.h"
#include "../../Objects/Player.h"
#include "../../Objects/Floor.h"
#include "../../Objects/Item.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>

// 定数定義
const float SCREEN_WIDTH = 1280.0f;
const float SCREEN_HEIGHT = 720.0f;
const float WORLD_WIDTH = MapData::MAP_WIDTH;

Floor g_floor;

// コンストラクタ
InGameScene::InGameScene()
    : currentPhase(GamePhase::Opening),
    allEvidenceCollected(false),
    dialogueSystem(nullptr),
    reasoningManager(nullptr),
    reasoningUI(nullptr),
    showResult(false),
    isCorrect(false),
    isGameOver(false),
    resultDisplayTime(0.0f),
    cameraX(0.0f),
    player1(nullptr),
    mainbgm(-1),
    se_success(-1),
    se_fail(-1),
    showNPCIndicator(true),
    charHandle_Player(-1),
    charHandle_Police(-1),
    charHandle_Fujisaki(-1),
    charHandle_Sasaki(-1),
    charHandle_Kimura(-1),
    charHandle_Yamada(-1),
    isSelectingEvidence(false),
    evidenceResultTimer(0.0f)
{
}

InGameScene::~InGameScene()
{
    Finalize();
}

void InGameScene::Initialize()
{
    player1 = new Player();
    player1->Initialize();
    g_floor.Initialize();

    // 証拠配置
    itemManager.Init();
    auto evidenceList = MapData::GetEvidenceList();
    for (const auto& evidence : evidenceList) {
        itemManager.Add(new Item(
            evidence.x, evidence.y,
            evidence.name, evidence.description,
            evidence.gameType
        ));
    }
    itemManager.Init();

    // NPC配置
    npcManager.Initialize();
    npcManager.AddSuspect(SuspectType::Fujisaki, MapData::AREA0_START + 400.0f, 560.0f);
    npcManager.AddSuspect(SuspectType::Kimura, MapData::AREA1_START + 400.0f, 560.0f);
    npcManager.AddSuspect(SuspectType::Sasaki, MapData::AREA2_START + 400.0f, 560.0f);
    npcManager.AddSuspect(SuspectType::Yamada, MapData::AREA4_START + 400.0f, 560.0f);

    for (const auto& npc : npcManager.GetAllNPCs()) {
        npc->Initialize();
    }

    dialogueSystem = new DialogueSystem();
    dialogueSystem->Initialize();

    LoadCharacterImages();

    // マネージャーの初期化
    backgroundManager.Initialize();
    areaTransitionManager.Initialize();
    gameTimer.Initialize(60.0f);
    miniGameManager.Initialize();

    reasoningManager = new ReasoningManager();
    reasoningManager->Initialize();
    reasoningManager->SetActive(false);

    reasoningUI = new ReasoningUI();
    reasoningUI->Initialize();

    currentPhase = GamePhase::Opening;

    cameraX = -240.0f;
    player1->SetX(100.0f);

    mainbgm = LoadSoundMem("Resource/Sound/BGM.mp3");
    se_success = LoadSoundMem("Resource/Sound/GetItem.mp3");
    se_fail = LoadSoundMem("Resource/Sound/footSE.mp3");

    // ミニゲームにSE設定
    miniGameManager.SetSoundEffects(se_success, se_fail);

    if (mainbgm != -1) {
        ChangeVolumeSoundMem(180, mainbgm);
        PlaySoundMem(mainbgm, DX_PLAYTYPE_LOOP);
    }

    SetupOpeningDialogue();
}

void InGameScene::LoadCharacterImages() {
    charHandle_Player = LoadGraph("Resource/Characters/Player/chara.png");
    charHandle_Police = LoadGraph("Resource/Characters/NPC/Police.png");
    charHandle_Fujisaki = LoadGraph("Resource/Characters/Suspects/Fujisaki.png");
    charHandle_Sasaki = LoadGraph("Resource/Characters/Suspects/Sasaki.png");
    charHandle_Kimura = LoadGraph("Resource/Characters/Suspects/Kimura.png");
    charHandle_Yamada = LoadGraph("Resource/Characters/Suspects/Yamada.png");
}

void InGameScene::SetupOpeningDialogue() {
    dialogueSystem->ClearDialogues();

    dialogueSystem->AddDialogue(DialogueData("", "深夜23:30、桜町駅前商店街\n終電を逃した天才探偵・神楽崎誠は、\n静まり返った商店街を歩いて帰路についていた。", CharacterPosition::Center));
    dialogueSystem->AddDialogue(DialogueData("", "そのとき――\n物音と悲鳴に気づき、雑貨店に駆け込むと\nそこには倒れた店主と、4人の人物が立っていた。", CharacterPosition::Center));
    dialogueSystem->AddDialogue(DialogueData("警察官", "全員動くな！現場検証するぞ！\n君も関係者だな。あっちで待機してくれ。", CharacterPosition::Right, charHandle_Player, charHandle_Police));
    dialogueSystem->AddDialogue(DialogueData("神楽崎", "待ってください。私は探偵です。\nこの事件、すでに犯人が誰かわかります。", CharacterPosition::Left, charHandle_Player, charHandle_Police));
    dialogueSystem->AddDialogue(DialogueData("警察官", "はぁ？何言ってんだお前。\nここは事件現場だ。民間人は帰れ。", CharacterPosition::Right, charHandle_Player, charHandle_Police));
    dialogueSystem->AddDialogue(DialogueData("神楽崎", "1分あれば、この事件を完璧に解決できます。", CharacterPosition::Left, charHandle_Player, charHandle_Police));
    dialogueSystem->AddDialogue(DialogueData("警察官", "......面白いこと言うな。\nじゃあやってみろ。", CharacterPosition::Right, charHandle_Player, charHandle_Police));
    dialogueSystem->AddDialogue(DialogueData("警察官", "1分だ。\n証拠を集めて犯人を当ててみせろ！", CharacterPosition::Right, charHandle_Player, charHandle_Police));
    dialogueSystem->AddDialogue(DialogueData("神楽崎", "（すでに犯人は分かっている...あとは証拠を集めて、証明するだけだ！）\nよし、調査開始だ！", CharacterPosition::Left, charHandle_Player, charHandle_Police));

    dialogueSystem->Start();
}

void InGameScene::SetupNPCDialogue(NPC* npc) {
    if (!npc) return;
    dialogueSystem->ClearDialogues();
    int targetHandle = -1;
    std::string npcName = npc->GetName();

    if (npcName.find("藤崎") != std::string::npos) targetHandle = charHandle_Fujisaki;
    else if (npcName.find("佐々木") != std::string::npos) targetHandle = charHandle_Sasaki;
    else if (npcName.find("木村") != std::string::npos) targetHandle = charHandle_Kimura;
    else if (npcName.find("山田") != std::string::npos) targetHandle = charHandle_Yamada;
    if (targetHandle == -1) targetHandle = charHandle_Police;

    const auto& testimonies = npc->GetAllTestimonies();
    for (const auto& testimony : testimonies) {
        dialogueSystem->AddDialogue(DialogueData(npcName, testimony.text, CharacterPosition::Right, charHandle_Player, targetHandle));
        if (testimony.isImportant) {
            dialogueSystem->AddDialogue(DialogueData("神楽崎", "（なるほど...これは重要な手がかりになりそうだ）", CharacterPosition::Left, charHandle_Player, targetHandle));
        }
    }
    dialogueSystem->Start();
}

eSceneType InGameScene::Update(float delta_second)
{
    InputManager* input = InputManager::GetInstance();

    // エリア遷移処理
    if (currentPhase == GamePhase::AreaTransition) {
        areaTransitionManager.Update(delta_second);
        if (!areaTransitionManager.IsTransitioning()) {
            currentPhase = GamePhase::EvidenceCollection;
            gameTimer.Resume();
        }
        return GetNowSceneType();
    }

    // オープニング・NPC会話処理
    if (currentPhase == GamePhase::Opening || currentPhase == GamePhase::NPCDialogue) {
        dialogueSystem->Update(delta_second);

        if (dialogueSystem->IsFinished()) {
            if (currentPhase == GamePhase::Opening) {
                currentPhase = GamePhase::EvidenceCollection;
                // タイマーは止めたまま
            }
            else {
                currentPhase = GamePhase::EvidenceCollection;
                gameTimer.Resume();
                npcManager.EndDialogue();
            }
        }
        return GetNowSceneType();
    }

    // 証拠収集フェーズ
    if (currentPhase == GamePhase::EvidenceCollection) {
        if (gameTimer.IsPaused()) {
            // Aボタンで開始
            if (input->GetKeyState(KEY_INPUT_Z) == eInputState::Pressed ||
                input->GetButtonState(XINPUT_BUTTON_A) == eInputState::Pressed) {
                gameTimer.Resume();
            }
            return GetNowSceneType();
        }

        if (itemManager.IsOpen()) {
            itemManager.Update(player1->GetX(), player1->GetY(), delta_second);
            if (input->GetKeyState(KEY_INPUT_E) == eInputState::Pressed) {
                itemManager.ToggleList();
            }
            return GetNowSceneType();
        }

        if (player1) player1->Update();
        
        // エリア遷移チェック
        if (areaTransitionManager.CheckAndStartTransition(player1->GetX())) {
            currentPhase = GamePhase::AreaTransition;
            gameTimer.Pause();
            int prevAreaIndex = areaTransitionManager.GetCurrentAreaIndex();
            // プレイヤー位置は遷移完了時に更新
        }

        if (input->GetKeyState(KEY_INPUT_A) == eInputState::Pressed ||
            input->GetButtonState(XINPUT_BUTTON_START) == eInputState::Pressed)
        {
            return eSceneType::eTitle;
        }

        if (input->GetKeyState(KEY_INPUT_E) == eInputState::Pressed ||
            input->GetButtonState(XINPUT_BUTTON_Y) == eInputState::Pressed) {
            itemManager.ToggleList();
        }

        if (input->GetKeyState(KEY_INPUT_TAB) == eInputState::Pressed ||
            input->GetButtonState(XINPUT_BUTTON_BACK) == eInputState::Pressed) {
            showNPCIndicator = !showNPCIndicator;
        }

        itemManager.Update(player1->GetX(), player1->GetY(), delta_second);
        npcManager.Update(player1->GetX(), player1->GetY(), delta_second);

        // NPC会話開始
        const auto& npcs = npcManager.GetAllNPCs();
        for (const auto& npc : npcs) {
            if (npc->IsInteracting() &&
                (input->GetKeyState(KEY_INPUT_Z) == eInputState::Pressed ||
                    input->GetButtonState(XINPUT_BUTTON_A) == eInputState::Pressed)) {

                currentPhase = GamePhase::NPCDialogue;
                gameTimer.Pause();

                npc->SetQuestioned(true);
                npc->SetState(NPCState::Talking);

                SetupNPCDialogue(npc.get());
                break;
            }
        }

        // アイテム取得
        const auto& items = itemManager.GetItems();
        for (Item* item : items) {
            if (item == nullptr) continue;

            if (!item->GetIsCollected()) {
                float dx = std::abs(player1->GetX() - item->GetX());
                float dy = std::abs(player1->GetY() - item->GetY());
                if (dx < 80.0f && dy < 150.0f) {
                    if (input->GetKeyState(KEY_INPUT_Z) == eInputState::Pressed ||
                        input->GetButtonState(XINPUT_BUTTON_A) == eInputState::Pressed)
                    {
                        currentPhase = GamePhase::MiniGame;
                        miniGameManager.Start(item);
                        break;
                    }
                }
            }
        }

        // カメラ位置更新
        float areaStartX = MapData::GetAreaStartX(areaTransitionManager.GetCurrentAreaIndex());
        cameraX = (areaStartX + 400.0f) - 640.0f;

        // タイマー更新
        gameTimer.Update(delta_second);

        // 全証拠収集チェック
        if (itemManager.GetCollectedCount() >= itemManager.GetTotalCount()) {
            allEvidenceCollected = true;
        }

        if (gameTimer.IsExpired() || allEvidenceCollected) {
            TransitionToReasoning();
        }
    }
    else if (currentPhase == GamePhase::MiniGame) {
        float remainingTime = gameTimer.GetRemainingTime();
        miniGameManager.Update(delta_second, remainingTime);
        
        // ミニゲームでタイムペナルティがあった場合
        if (remainingTime < gameTimer.GetRemainingTime()) {
            gameTimer.SubtractTime(gameTimer.GetRemainingTime() - remainingTime);
        }
        
        if (!miniGameManager.IsActive()) {
            currentPhase = GamePhase::EvidenceCollection;
            gameTimer.Resume();
        }
        
        gameTimer.Update(delta_second);
        if (gameTimer.IsExpired()) {
            TransitionToReasoning();
        }
    }
    else if (currentPhase == GamePhase::Reasoning) {
        if (evidenceResultTimer > 0.0f) {
            evidenceResultTimer -= delta_second;
            return GetNowSceneType();
        }

        if (isSelectingEvidence) {
            itemManager.Update(0, 0, delta_second);
            if (input->GetKeyState(KEY_INPUT_E) == eInputState::Pressed ||
                input->GetKeyState(KEY_INPUT_X) == eInputState::Pressed ||
                input->GetButtonState(XINPUT_BUTTON_B) == eInputState::Pressed) {
                itemManager.ToggleList();
                isSelectingEvidence = false;
            }
            if (input->GetKeyState(KEY_INPUT_Z) == eInputState::Pressed ||
                input->GetButtonState(XINPUT_BUTTON_A) == eInputState::Pressed) {
                const Item* selectedItem = itemManager.GetSelectedItem();
                if (selectedItem && selectedItem->GetIsCollected()) {
                    int result = reasoningManager->TryEliminateWithEvidence(selectedItem->GetName());
                    if (result == 0) {
                        evidenceResultMessage = "成功";
                        if (se_success != -1) PlaySoundMem(se_success, DX_PLAYTYPE_BACK);
                    }
                    else if (result == 1) {
                        evidenceResultMessage = "その証拠では矛盾を示せないようだ...（ライフ減少）";
                        reasoningManager->DamageLife();
                        if (se_fail != -1) PlaySoundMem(se_fail, DX_PLAYTYPE_BACK);
                    }
                    else {
                        evidenceResultMessage = "それは除外できない（または既に除外済み）";
                    }
                    evidenceResultTimer = 1.5f;
                    itemManager.ToggleList();
                    isSelectingEvidence = false;
                }
            }
            return GetNowSceneType();
        }

        if (showResult) {
            resultDisplayTime += delta_second;
            if (resultDisplayTime >= 3.0f) {
                return eSceneType::eTitle;
            }
        }
        else if (reasoningManager) {
            reasoningManager->Update(delta_second);

            if (input->GetKeyState(KEY_INPUT_X) == eInputState::Pressed ||
                input->GetButtonState(XINPUT_BUTTON_Y) == eInputState::Pressed) {
                isSelectingEvidence = true;
                itemManager.ToggleList();
            }

            if (reasoningManager->IsGameOver()) {
                isCorrect = false;
                isGameOver = true;
                showResult = true;
                resultDisplayTime = 0.0f;
                if (se_fail != -1) PlaySoundMem(se_fail, DX_PLAYTYPE_BACK);
            }
            else if (reasoningManager->IsGameClear()) {
                isCorrect = true;
                isGameOver = false;
                showResult = true;
                resultDisplayTime = 0.0f;
                if (se_success != -1) PlaySoundMem(se_success, DX_PLAYTYPE_BACK);
            }
        }
    }

    return GetNowSceneType();
}

void InGameScene::Draw() const
{
    int currentAreaIndex = areaTransitionManager.GetCurrentAreaIndex();
    
    // 背景描画
    if (currentPhase == GamePhase::Opening || currentPhase == GamePhase::NPCDialogue) {
        backgroundManager.DrawBackground(currentAreaIndex, true);
    }
    else {
        backgroundManager.DrawBackground(currentAreaIndex, false);
        backgroundManager.DrawBackgroundObjects(cameraX);
    }

    if (currentPhase == GamePhase::Opening) {
        if (dialogueSystem) dialogueSystem->Draw();
    }
    else if (currentPhase == GamePhase::NPCDialogue) {
        g_floor.Draw(cameraX);
        npcManager.Draw(cameraX);
        itemManager.Draw(cameraX);
        if (player1) player1->Draw(cameraX);
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
        DrawBox(0, 0, 1280, 720, GetColor(0, 0, 0), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        if (dialogueSystem) dialogueSystem->Draw();
    }
    else if (currentPhase == GamePhase::EvidenceCollection) {
        g_floor.Draw(cameraX);
        npcManager.Draw(cameraX);
        itemManager.Draw(cameraX);
        if (player1) player1->Draw(cameraX);

        // 事件概要表示（タイマー停止中）
        if (gameTimer.IsPaused()) {
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
            DrawBox(0, 0, 1280, 720, GetColor(0, 0, 0), TRUE);
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

            DrawBox(140, 100, 1140, 620, GetColor(30, 30, 40), TRUE);
            DrawBox(140, 100, 1140, 620, GetColor(255, 255, 255), FALSE);

            SetFontSize(32);
            DrawFormatString(200, 140, GetColor(255, 200, 50), "【 事件概要 】");
            SetFontSize(24);
            DrawFormatString(200, 200, GetColor(255, 255, 255), "被害者：店主　　現場：ミナト雑貨店");
            DrawFormatString(200, 240, GetColor(255, 255, 255), "容疑者：現場にいた4人の人物");

            DrawFormatString(200, 300, GetColor(200, 200, 255), "■ 任務");
            DrawFormatString(220, 340, GetColor(255, 255, 255), "・制限時間60秒以内に証拠を集める");
            DrawFormatString(220, 380, GetColor(255, 255, 255), "・容疑者から話を聞く（Aボタン）");
            DrawFormatString(220, 420, GetColor(255, 255, 255), "・集めた証拠で矛盾を暴き、犯人を特定する");

            DrawFormatString(400, 520, GetColor(255, 255, 0), "Aボタン を押して調査開始！");
            SetFontSize(16);
        }
        else if (!itemManager.IsOpen()) {
            gameTimer.Draw();
            DrawPhaseInfo();
            const char* areas[] = { "裏口", "店内奥", "レジ周辺", "店頭", "店外" };
            int areaIdx = currentAreaIndex;
            if (areaIdx < 0) areaIdx = 0; if (areaIdx > 4) areaIdx = 4;
            DrawFormatString(240, 20, GetColor(200, 255, 255), "現在地: %s", areas[areaIdx]);

            if (currentAreaIndex < 4) DrawFormatString(1280 - 150, 360, GetColor(255, 255, 0), "エリア移動 →");
            if (currentAreaIndex > 0) DrawFormatString(50, 360, GetColor(255, 255, 0), "← エリア移動");

            if (showNPCIndicator && !npcManager.IsDialogueActive()) {
                DrawNPCIndicator();
            }
        }
    }
    else if (currentPhase == GamePhase::AreaTransition) {
        g_floor.Draw(cameraX);
        itemManager.Draw(cameraX);
        if (player1) player1->Draw(cameraX);
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)areaTransitionManager.GetFadeAlpha());
        DrawBox(0, 0, 1280, 720, GetColor(0, 0, 0), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }
    else if (currentPhase == GamePhase::MiniGame) {
        g_floor.Draw(cameraX);
        itemManager.Draw(cameraX);
        if (player1) player1->Draw(cameraX);
        gameTimer.Draw();
        miniGameManager.Draw();
    }
    else if (currentPhase == GamePhase::Reasoning) {
        if (showResult) {
            DrawResult();
        }
        else {
            int bgImage = backgroundManager.GetMainBackgroundImage();
            if (bgImage != -1) {
                SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
                DrawGraph(0, 0, bgImage, TRUE);
                SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
            }
            if (reasoningUI && reasoningManager) {
                reasoningUI->Draw(reasoningManager);
            }

            // 証拠選択モードならリストを表示
            if (isSelectingEvidence) {
                itemManager.Draw(cameraX);
                DrawFormatString(140, 580, GetColor(255, 255, 0), "矛盾する証拠を選んで Aボタンで決定！");
            }

            // 結果メッセージ表示
            if (evidenceResultTimer > 0.0f) {
                DrawBox(300, 300, 980, 420, GetColor(0, 0, 0), TRUE);
                DrawBox(300, 300, 980, 420, GetColor(255, 255, 255), FALSE);
                DrawFormatString(350, 350, GetColor(255, 255, 255), "%s", evidenceResultMessage.c_str());
            }
        }
    }

    if (itemManager.IsOpen() && currentPhase != GamePhase::Reasoning) {
        itemManager.Draw(cameraX);
    }
}

void InGameScene::DrawNPCIndicator() const {
    if (!player1) return;
    float playerX = player1->GetX();
    const auto& npcs = npcManager.GetAllNPCs();
    int indicatorY = 120;
    int indicatorHeight = 60;
    DrawBox(20, indicatorY, 320, indicatorY + indicatorHeight, GetColor(0, 0, 0), TRUE);
    DrawBox(20, indicatorY, 320, indicatorY + indicatorHeight, GetColor(100, 100, 100), FALSE);
    DrawFormatString(30, indicatorY + 5, GetColor(255, 200, 100), "容疑者の位置");
    int yOffset = indicatorY + 30;
    for (const auto& npc : npcs) {
        float npcX = npc->GetX();
        float distance = abs(npcX - playerX);
        unsigned int color = GetColor(255, 255, 255);
        std::string direction = "";
        if (distance < 100.0f) { color = GetColor(100, 255, 100); direction = "★"; }
        else if (npcX > playerX) { color = GetColor(200, 200, 255); direction = "→"; }
        else { color = GetColor(255, 200, 200); direction = "←"; }
        std::string mark = npc->HasBeenQuestioned() ? "[済]" : "";
        DrawFormatString(30, yOffset, color, "%s %s %s", direction.c_str(), npc->GetName().c_str(), mark.c_str());
        yOffset += 20;
        if (yOffset > indicatorY + indicatorHeight - 10) break;
    }
}

int InGameScene::CountQuestionedNPCs() const {
    int count = 0;
    const auto& npcs = npcManager.GetAllNPCs();
    for (const auto& npc : npcs) {
        if (npc->HasBeenQuestioned()) count++;
    }
    return count;
}

void InGameScene::Finalize() {
    if (mainbgm >= 0) {
        StopSoundMem(mainbgm);
        DeleteSoundMem(mainbgm);
        mainbgm = -1;
    }
    if (se_success != -1) {
        DeleteSoundMem(se_success);
        se_success = -1;
    }
    if (se_fail != -1) {
        DeleteSoundMem(se_fail);
        se_fail = -1;
    }
    
    // BackgroundManagerが自動で解放するのでここでは不要
    // backgroundManager.Finalize() はデストラクタで呼ばれる

    // キャラクター画像の削除
    if (charHandle_Player != -1) { DeleteGraph(charHandle_Player); charHandle_Player = -1; }
    if (charHandle_Police != -1) { DeleteGraph(charHandle_Police); charHandle_Police = -1; }
    if (charHandle_Fujisaki != -1) { DeleteGraph(charHandle_Fujisaki); charHandle_Fujisaki = -1; }
    if (charHandle_Sasaki != -1) { DeleteGraph(charHandle_Sasaki); charHandle_Sasaki = -1; }
    if (charHandle_Kimura != -1) { DeleteGraph(charHandle_Kimura); charHandle_Kimura = -1; }
    if (charHandle_Yamada != -1) { DeleteGraph(charHandle_Yamada); charHandle_Yamada = -1; }

    if (player1) { delete player1; player1 = nullptr; }
    if (reasoningManager) { delete reasoningManager; reasoningManager = nullptr; }
    if (reasoningUI) { delete reasoningUI; reasoningUI = nullptr; }
    if (dialogueSystem) { delete dialogueSystem; dialogueSystem = nullptr; }
}

eSceneType InGameScene::GetNowSceneType() const { return eSceneType::eInGame; }

void InGameScene::TransitionToReasoning() {
    if (player1) player1->StopAudio();
    currentPhase = GamePhase::Reasoning;
    if (reasoningManager) {
        std::vector<std::string> collected = itemManager.GetCollectedItems();
        reasoningManager->SetActive(true);
    }
}

void InGameScene::DrawPhaseInfo() const {
    DrawFormatString(20, 50, GetColor(200, 200, 200), "証拠発見数: %d / %d  容疑者: 質問済み %d/4",
        itemManager.GetCollectedCount(), itemManager.GetTotalCount(), CountQuestionedNPCs());
}

void InGameScene::DrawResult() const {
    int x = 200, y = 150;
    int boxWidth = 880, boxHeight = 420;
    DrawBox(x - 20, y - 20, x + boxWidth, y + boxHeight, GetColor(0, 0, 0), TRUE);
    DrawBox(x - 20, y - 20, x + boxWidth, y + boxHeight, GetColor(255, 255, 255), FALSE);
    if (isCorrect) {
        SetFontSize(40);
        DrawFormatString(x + 280, y + 20, GetColor(0, 255, 0), "CASE CLOSED");
        SetFontSize(24);
        DrawFormatString(x + 320, y + 70, GetColor(255, 255, 0), "事件解決！");
        SetFontSize(16);
        DrawFormatString(x + 40, y + 150, GetColor(255, 255, 255), "おめでとうございます！真実にたどり着きました。");
    }
    else {
        SetFontSize(40);
        DrawFormatString(x + 280, y + 20, GetColor(255, 0, 0), "CASE FAILED");
        SetFontSize(24);
        DrawFormatString(x + 320, y + 70, GetColor(255, 150, 0), "推理失敗...");
        SetFontSize(16);
        DrawFormatString(x + 40, y + 150, GetColor(255, 255, 255), "推理が間違っています。もう一度証拠を集めましょう。");
    }
}

void InGameScene::DrawOpening() const {}
void InGameScene::DrawAreaInfo() const {}