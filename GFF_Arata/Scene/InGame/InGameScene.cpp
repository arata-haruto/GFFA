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
    timeLimit(60.0f),
    remainingTime(60.0f),
    timerPaused(true),
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
    mg_resultTimer(0.0f),
    mg_lastResultSuccess(false),
    currentAreaIndex(0),
    mainbgm(-1),
    se_success(-1),
    se_fail(-1),
    showNPCIndicator(true),
    back_ground_image(-1),
    charHandle_Player(-1),
    charHandle_Police(-1),
    charHandle_Fujisaki(-1),
    charHandle_Sasaki(-1),
    charHandle_Kimura(-1),
    charHandle_Yamada(-1),
    currentInteractingItem(nullptr),
    mg_targetItem(nullptr),
    mg_barPosition(0.0f),
    mg_barSpeed(0.0f),
    mg_targetMin(0.0f),
    mg_targetMax(0.0f),
    isSelectingEvidence(false),
    evidenceResultTimer(0.0f)
{
    for (int i = 0; i < 4; i++) bgHandles[i] = -1;
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
    LoadBackgroundObjects();

    reasoningManager = new ReasoningManager();
    reasoningManager->Initialize();
    reasoningManager->SetActive(false);

    reasoningUI = new ReasoningUI();
    reasoningUI->Initialize();

    currentPhase = GamePhase::Opening;
    timerPaused = true;

    currentAreaIndex = 0;
    cameraX = -240.0f;
    player1->SetX(100.0f);

    back_ground_image = LoadGraph("Resource/Background/Shop_BG.png");

    if (back_ground_image == -1) {
        bgHandles[0] = LoadGraph("Resource/Background/BG.jpg");
        bgHandles[1] = LoadGraph("Resource/Background/BG.jpg");
        bgHandles[2] = LoadGraph("Resource/Background/BG.jpg");
        bgHandles[3] = LoadGraph("Resource/Background/BG.jpg");
        bgHandles[4] = LoadGraph("Resource/Background/BG.jpg");
    }

    mainbgm = LoadSoundMem("Resource/Sound/BGM.mp3");
    se_success = LoadSoundMem("Resource/Sound/GetItem.mp3");
    se_fail = LoadSoundMem("Resource/Sound/footSE.mp3");

    if (mainbgm != -1) {
        ChangeVolumeSoundMem(180, mainbgm);
        PlaySoundMem(mainbgm, DX_PLAYTYPE_LOOP);
    }

    SetupOpeningDialogue();
}

void InGameScene::LoadCharacterImages() {
    // 主人公の立ち絵画像を読み込み（会話システム用）
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

void InGameScene::LoadBackgroundObjects()
{
    backgroundObjects.clear();
    auto objects = MapData::GetBackgroundObjects();
    for (const auto& obj : objects) {
        int handle = -1;
        if (!obj.imagePath.empty()) {
            handle = LoadGraph(obj.imagePath.c_str());
        }
        backgroundObjects.push_back({ handle, obj.x, obj.y, obj.width, obj.height });
    }
}

eSceneType InGameScene::Update(float delta_second)
{
    InputManager* input = InputManager::GetInstance();

    if (currentPhase == GamePhase::AreaTransition) {
        UpdateAreaTransition(delta_second);
        return GetNowSceneType();
    }

    if (currentPhase == GamePhase::Opening || currentPhase == GamePhase::NPCDialogue) {
        dialogueSystem->Update(delta_second);

        if (dialogueSystem->IsFinished()) {
            if (currentPhase == GamePhase::Opening) {
             
                currentPhase = GamePhase::EvidenceCollection;
                timerPaused = true; // タイマーは止めたまま

          
            }
            else {
                currentPhase = GamePhase::EvidenceCollection;
                timerPaused = false;
                npcManager.EndDialogue();
            }
        }
        return GetNowSceneType();
    }

    if (currentPhase == GamePhase::EvidenceCollection) {
        if (timerPaused) {
            // Aボタンで開始
            if (input->GetKeyState(KEY_INPUT_Z) == eInputState::Pressed ||
                input->GetButtonState(XINPUT_BUTTON_A) == eInputState::Pressed) {
                timerPaused = false;
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
        CheckAreaTransition();

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

        const auto& npcs = npcManager.GetAllNPCs();
        for (const auto& npc : npcs) {
            if (npc->IsInteracting() &&
                (input->GetKeyState(KEY_INPUT_Z) == eInputState::Pressed ||
                    input->GetButtonState(XINPUT_BUTTON_A) == eInputState::Pressed)) {

                currentPhase = GamePhase::NPCDialogue;
                timerPaused = true;

                npc->SetQuestioned(true);
                npc->SetState(NPCState::Talking);

                SetupNPCDialogue(npc.get());
                break;
            }
        }

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
                        StartMiniGame(item);
                        break;
                    }
                }
            }
        }

        float areaStartX = MapData::GetAreaStartX(currentAreaIndex);
        cameraX = (areaStartX + 400.0f) - 640.0f;

        if (!timerPaused) {
            remainingTime -= delta_second;
            if (remainingTime < 0.0f) remainingTime = 0.0f;
        }

        if (itemManager.GetCollectedCount() >= itemManager.GetTotalCount()) {
            allEvidenceCollected = true;
        }

        if (remainingTime <= 0.0f || allEvidenceCollected) {
            TransitionToReasoning();
        }
    }
    else if (currentPhase == GamePhase::MiniGame) {
        UpdateMiniGame(delta_second);
        if (!timerPaused) {
            remainingTime -= delta_second;
            if (remainingTime <= 0.0f) {
                remainingTime = 0.0f;
                TransitionToReasoning();
            }
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

// ... CheckAreaTransition, ChangeArea, UpdateAreaTransition ...
void InGameScene::CheckAreaTransition() {
    float playerX = player1->GetX();
    float areaStartX = MapData::GetAreaStartX(currentAreaIndex);
    float areaEndX = areaStartX + MapData::AREA_WIDTH;
    if (playerX > areaEndX - 30.0f && currentAreaIndex < 4) ChangeArea(currentAreaIndex + 1);
    else if (playerX < areaStartX + 30.0f && currentAreaIndex > 0) ChangeArea(currentAreaIndex - 1);
}

void InGameScene::ChangeArea(int newAreaIndex) {
    currentPhase = GamePhase::AreaTransition;
    nextAreaIndex = newAreaIndex;
    isFadingOut = true;
    fadeAlpha = 0.0f;
    timerPaused = true;
}

void InGameScene::UpdateAreaTransition(float delta_second) {
    float fadeSpeed = 300.0f;
    if (isFadingOut) {
        fadeAlpha += fadeSpeed * delta_second;
        if (fadeAlpha >= 255.0f) {
            fadeAlpha = 255.0f;
            isFadingOut = false;
            int prevAreaIndex = currentAreaIndex;
            currentAreaIndex = nextAreaIndex;
            float newAreaStartX = MapData::GetAreaStartX(currentAreaIndex);
            float newAreaEndX = newAreaStartX + MapData::AREA_WIDTH;
            if (prevAreaIndex < currentAreaIndex) player1->SetX(newAreaStartX + 100.0f);
            else player1->SetX(newAreaEndX - 100.0f);
        }
    }
    else {
        fadeAlpha -= fadeSpeed * delta_second;
        if (fadeAlpha <= 0.0f) {
            fadeAlpha = 0.0f;
            currentPhase = GamePhase::EvidenceCollection;
            timerPaused = false;
        }
    }
}

void InGameScene::Draw() const
{
    // 背景描画
    if (currentPhase == GamePhase::Opening || currentPhase == GamePhase::NPCDialogue) {
        DrawBox(0, 0, 1280, 720, GetColor(0, 0, 0), TRUE);
        int handle = bgHandles[currentAreaIndex];
        if (handle != -1) {
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, 80);
            DrawGraph(0, 0, handle, TRUE);
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        }
    }
    else {
        if (back_ground_image != -1) {
            int bgW, bgH;
            GetGraphSize(back_ground_image, &bgW, &bgH);
            int srcX = currentAreaIndex * 800;
            int srcW = 800;
            int drawX = (1280 - 800) / 2;

            if (currentAreaIndex == 0) SetDrawBright(200, 200, 200);
            else if (currentAreaIndex == 1) SetDrawBright(220, 220, 255);
            else if (currentAreaIndex == 2) SetDrawBright(255, 255, 200);
            else if (currentAreaIndex == 3) SetDrawBright(255, 200, 200);
            else SetDrawBright(100, 100, 150);

            DrawRectGraph(drawX, 0, srcX, 0, srcW, SCREEN_HEIGHT, back_ground_image, TRUE, FALSE);
            SetDrawBright(255, 255, 255);
        }
        else {
            int handle = bgHandles[currentAreaIndex];
            if (handle != -1) DrawGraph(0, 0, handle, TRUE);
        }
        DrawBackgroundObjects(cameraX);
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

        // ★追加: 事件概要表示（タイマー停止中）
        if (timerPaused) {
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
            DrawTimer();
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
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)fadeAlpha);
        DrawBox(0, 0, 1280, 720, GetColor(0, 0, 0), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }
    else if (currentPhase == GamePhase::MiniGame) {
        g_floor.Draw(cameraX);
        itemManager.Draw(cameraX);
        if (player1) player1->Draw(cameraX);
        DrawTimer();
        DrawMiniGame();
    }
    else if (currentPhase == GamePhase::Reasoning) {
        if (showResult) {
            DrawResult();
        }
        else {
            if (back_ground_image != -1) {
                SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
                DrawGraph(0, 0, back_ground_image, TRUE);
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


void InGameScene::DrawBackgroundObjects(float cameraOffsetX) const {
    for (const auto& obj : backgroundObjects) {
        float drawX = obj.x - cameraOffsetX;
        if (drawX + obj.width < 0 || drawX > SCREEN_WIDTH) continue;
        if (obj.handle != -1) {
            DrawExtendGraph((int)drawX, (int)obj.y, (int)(drawX + obj.width), (int)(obj.y + obj.height), obj.handle, TRUE);
        }
        else {
            DrawBox((int)drawX, (int)obj.y, (int)(drawX + obj.width), (int)(obj.y + obj.height), GetColor(100, 100, 100), TRUE);
        }
    }
}

void InGameScene::DrawAreaInfo() const { /*...*/ }
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
    if (back_ground_image >= 0) {
        DeleteGraph(back_ground_image);
        back_ground_image = -1;
    }
    for (int i = 0; i < 5; i++) {
        if (bgHandles[i] != -1) {
            DeleteGraph(bgHandles[i]);
            bgHandles[i] = -1;
        }
    }
    for (auto& obj : backgroundObjects) {
        if (obj.handle != -1) DeleteGraph(obj.handle);
    }
    backgroundObjects.clear();
    
    // キャラクター画像の削除
    if (charHandle_Player != -1) {
        DeleteGraph(charHandle_Player);
        charHandle_Player = -1;
    }
    if (charHandle_Police != -1) {
        DeleteGraph(charHandle_Police);
        charHandle_Police = -1;
    }
    if (charHandle_Fujisaki != -1) {
        DeleteGraph(charHandle_Fujisaki);
        charHandle_Fujisaki = -1;
    }
    if (charHandle_Sasaki != -1) {
        DeleteGraph(charHandle_Sasaki);
        charHandle_Sasaki = -1;
    }
    if (charHandle_Kimura != -1) {
        DeleteGraph(charHandle_Kimura);
        charHandle_Kimura = -1;
    }
    if (charHandle_Yamada != -1) {
        DeleteGraph(charHandle_Yamada);
        charHandle_Yamada = -1;
    }
    
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
        //reasoningManager->FilterOptions(collected);
        reasoningManager->SetActive(true);
    }
}
void InGameScene::StartMiniGame(Item* item) {
    currentPhase = GamePhase::MiniGame;
    mg_targetItem = item;
    mg_barPosition = 0.0f;
    mg_barSpeed = 200.0f;
    mg_targetMin = 40.0f;
    mg_targetMax = 60.0f;
    mg_resultTimer = 0.0f;
}
void InGameScene::OnMiniGameComplete(bool success, Item* item) {
    if (success && item) {
        item->SetCollected(true);
    }
    currentInteractingItem = nullptr;
    currentPhase = GamePhase::EvidenceCollection;
    timerPaused = false;
}
void InGameScene::UpdateMiniGame(float delta_second) {
    if (mg_resultTimer > 0.0f) {
        mg_resultTimer -= delta_second;
        if (mg_resultTimer <= 0.0f) {
            OnMiniGameComplete(mg_lastResultSuccess, mg_targetItem);
        }
        return;
    }
    InputManager* input = InputManager::GetInstance();
    mg_barPosition += mg_barSpeed * delta_second;
    if (mg_barPosition > 100.0f) mg_barPosition = 0.0f;
    if (input->GetKeyState(KEY_INPUT_Z) == eInputState::Pressed ||
        input->GetButtonState(XINPUT_BUTTON_A) == eInputState::Pressed)
    {
        if (mg_barPosition >= mg_targetMin && mg_barPosition <= mg_targetMax) {
            if (se_success != -1) PlaySoundMem(se_success, DX_PLAYTYPE_BACK);
            mg_lastResultSuccess = true;
        }
        else {
            if (se_fail != -1) PlaySoundMem(se_fail, DX_PLAYTYPE_BACK);
            remainingTime -= 10.0f;
            mg_lastResultSuccess = false;
        }
        mg_resultTimer = 1.0f;
    }
}
void InGameScene::DrawMiniGame() const {
    int cx = (int)SCREEN_WIDTH / 2;
    int cy = (int)SCREEN_HEIGHT / 2;
    int w = 400;
    int h = 60;
    DrawBox(cx - w / 2 - 5, cy - h / 2 - 5, cx + w / 2 + 5, cy + h / 2 + 5, GetColor(255, 255, 255), FALSE);
    DrawBox(cx - w / 2, cy - h / 2, cx + w / 2, cy + h / 2, GetColor(30, 30, 30), TRUE);
    if (mg_resultTimer > 0.0f) {
        if (mg_lastResultSuccess) {
            DrawFormatString(cx - 60, cy - 10, GetColor(0, 255, 0), "GET EVIDENCE!");
        }
        else {
            DrawFormatString(cx - 40, cy - 10, GetColor(255, 0, 0), "FAILED...");
            DrawFormatString(cx - 50, cy + 20, GetColor(255, 100, 100), "-10 Seconds");
        }
        return;
    }
    if (mg_targetItem) {
        DrawFormatString(cx - 100, cy - 60, GetColor(255, 255, 0), "TARGET: %s", mg_targetItem->GetName().c_str());
    }
    int z1 = cx - w / 2 + (int)(w * (mg_targetMin / 100.0f));
    int z2 = cx - w / 2 + (int)(w * (mg_targetMax / 100.0f));
    DrawBox(z1, cy - h / 2, z2, cy + h / 2, GetColor(0, 255, 0), TRUE);
    int bx = cx - w / 2 + (int)(w * (mg_barPosition / 100.0f));
    DrawBox(bx - 3, cy - h / 2 - 5, bx + 3, cy + h / 2 + 5, GetColor(255, 50, 50), TRUE);
    DrawFormatString(cx - 80, cy + 40, GetColor(255, 255, 255), "Aボタンでタイミングよく止めろ！");
}
void InGameScene::DrawTimer() const {
    int x = 20, y = 20;
    int boxWidth = 300, boxHeight = 80;
    DrawBox(x - 10, y - 10, x + boxWidth, y + boxHeight, GetColor(0, 0, 0), TRUE);
    DrawBox(x - 10, y - 10, x + boxWidth, y + boxHeight, GetColor(100, 100, 100), FALSE);
    int minutes = (int)(remainingTime / 60.0f);
    int seconds = (int)(remainingTime) % 60;
    unsigned int color;
    if (remainingTime < 10.0f) {
        color = GetColor(255, 0, 0);
        if (((int)(remainingTime * 4)) % 2 == 0) color = GetColor(255, 255, 0);
    }
    else if (remainingTime < 30.0f) color = GetColor(255, 150, 0);
    else color = GetColor(255, 255, 255);
    SetFontSize(32);
    DrawFormatString(x, y, color, "Time: %02d:%02d", minutes, seconds);
    SetFontSize(16);
    if (remainingTime <= 40.0f && remainingTime > 30.0f) DrawFormatString(x, y + 45, GetColor(200, 200, 200), "警察官：「まだ余裕か？」");
    else if (remainingTime <= 30.0f && remainingTime > 20.0f) DrawFormatString(x, y + 45, GetColor(255, 150, 0), "警察官：「あと30秒だぞ！」");
    else if (remainingTime <= 20.0f && remainingTime > 10.0f) DrawFormatString(x, y + 45, GetColor(255, 100, 0), "警察官：「急げ！20秒だ！」");
    else if (remainingTime <= 10.0f) DrawFormatString(x, y + 45, GetColor(255, 0, 0), "警察官：「時間だ！答えろ！」");
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