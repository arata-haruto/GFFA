#include "InGameScene.h"
#include "../SceneFactory.h"
#include "../../Utility/InputManager.h"
#include "DxLib.h"
#include <cmath>

InGameScene::InGameScene()
    : currentPhase(GamePhase::Opening),
    timeLimit(60.0f),
    remainingTime(60.0f),
    allEvidenceCollected(false),
    player1(nullptr),
    floor(nullptr),
    reasoningManager(nullptr),
    reasoningUI(nullptr),
    showResult(false),
    isCorrect(false),
    resultDisplayTime(0.0f),
    cameraX(0.0f),
    mainbgm(-1),
    bg_backdoor(-1),
    bg_storeinner(-1),
    bg_cashregister(-1),
    bg_storefront(-1),
    bg_outside(-1)
{
}

void InGameScene::Initialize()
{
    __super::Initialize();

    printfDx("InGameScene初期化開始...\n");

    // プレイヤー初期化
    player1 = new Player();
    player1->Initialize();

    // 床初期化
    floor = new Floor();
    floor->Initialize();

    // 証拠を配置（8個）
    itemManager.Add(new Item(400.0f, 500.0f, "血のついた金属バット", "凶器と思われる", MiniGameType::None));
    itemManager.Add(new Item(1800.0f, 500.0f, "防犯カメラの映像", "23:15に人影が映っている", MiniGameType::None));
    itemManager.Add(new Item(2400.0f, 500.0f, "店主の手帳", "「弟と話し合い」のメモ", MiniGameType::None));
    itemManager.Add(new Item(3600.0f, 480.0f, "壊れたレジの記録", "現金が残っている", MiniGameType::None));
    itemManager.Add(new Item(4200.0f, 520.0f, "散乱した商品", "激しい争いの痕跡", MiniGameType::None));
    itemManager.Add(new Item(5200.0f, 490.0f, "割れた窓ガラス", "内側から割られている", MiniGameType::None));
    itemManager.Add(new Item(6800.0f, 510.0f, "目撃者の証言", "女性を目撃したという", MiniGameType::None));
    itemManager.Add(new Item(7400.0f, 500.0f, "タバコの吸い殻", "被害者は非喫煙者", MiniGameType::None));

    itemManager.Init();

    // 推理システム初期化
    reasoningManager = new ReasoningManager();
    reasoningManager->Initialize();
    reasoningManager->SetActive(false);

    reasoningUI = new ReasoningUI();
    reasoningUI->Initialize();

    currentPhase = GamePhase::Opening;
    cameraX = 0.0f;
    showResult = false;
    isCorrect = false;

    // BGM読み込み
    mainbgm = LoadSoundMem("Resource/Sound/BGM.mp3");
    if (mainbgm != -1) {
        PlaySoundMem(mainbgm, DX_PLAYTYPE_LOOP);
    }

    // 背景画像読み込み（エリアごと）
    bg_backdoor = LoadGraph("Resource/Background/裏口.png");
    bg_storeinner = LoadGraph("Resource/Background/店内奥.png");
    bg_cashregister = LoadGraph("Resource/Background/レジ周辺.png");
    bg_storefront = LoadGraph("Resource/Background/店頭.png");
    bg_outside = LoadGraph("Resource/Background/店外.png");

    printfDx("InGameScene初期化完了！\n");
}

eSceneType InGameScene::Update(float delta_second)
{
    InputManager* input = InputManager::GetInstance();

    if (currentPhase == GamePhase::Opening) {
        if (input->GetKeyState(KEY_INPUT_Z) == eInputState::Pressed ||
            input->GetButtonState(XINPUT_BUTTON_A) == eInputState::Pressed)
        {
            currentPhase = GamePhase::EvidenceCollection;
        }

        if (input->GetKeyState(KEY_INPUT_ESCAPE) == eInputState::Pressed) {
            return eSceneType::eTitle;
        }
    }
    else if (currentPhase == GamePhase::EvidenceCollection) {
        if (player1) player1->Update();

        if (input->GetKeyState(KEY_INPUT_ESCAPE) == eInputState::Pressed) {
            return eSceneType::eTitle;
        }

        if (input->GetKeyState(KEY_INPUT_E) == eInputState::Pressed) {
            itemManager.ToggleList();
        }

        itemManager.Update(player1->GetX(), player1->GetY(), delta_second);

        // カメラ追従
        const float screenW = 1280.0f;
        const float mapWidth = 8000.0f;
        float playerX = player1->GetX();

        float targetCameraX = playerX - screenW / 2.0f;
        if (targetCameraX < 0.0f) targetCameraX = 0.0f;
        if (targetCameraX > mapWidth - screenW) targetCameraX = mapWidth - screenW;

        cameraX = targetCameraX;

        // タイマー更新
        remainingTime -= delta_second;
        if (remainingTime < 0.0f) remainingTime = 0.0f;

        // 証拠収集完了チェック
        if (itemManager.GetCollectedCount() >= itemManager.GetTotalCount()) {
            allEvidenceCollected = true;
        }

        // 推理フェーズへ移行
        if (remainingTime <= 0.0f || allEvidenceCollected) {
            TransitionToReasoning();
        }
    }
    else if (currentPhase == GamePhase::Reasoning) {
        if (showResult) {
            resultDisplayTime += delta_second;
            if (resultDisplayTime >= 5.0f) {
                if (input->GetKeyState(KEY_INPUT_Z) == eInputState::Pressed ||
                    input->GetButtonState(XINPUT_BUTTON_A) == eInputState::Pressed) {
                    return eSceneType::eTitle;
                }
            }
        }
        else if (reasoningManager) {
            reasoningManager->Update(delta_second);

            if (reasoningManager->IsGameClear()) {
                showResult = true;
                isCorrect = true;
                resultDisplayTime = 0.0f;
                printfDx("ゲームクリア！\n");
            }
            else if (reasoningManager->IsGameOver()) {
                showResult = true;
                isCorrect = false;
                resultDisplayTime = 0.0f;
                printfDx("ゲームオーバー...\n");
            }
        }

        if (input->GetKeyState(KEY_INPUT_ESCAPE) == eInputState::Pressed) {
            return eSceneType::eTitle;
        }
    }

    return GetNowSceneType();
}

void InGameScene::Draw() const
{
    if (currentPhase == GamePhase::Opening) {
        DrawOpening();
    }
    else if (currentPhase == GamePhase::EvidenceCollection) {
        // 背景描画（エリアごと）
        // エリア境界: 0-800(裏口), 800-1600(店内奥), 1600-2400(レジ), 2400-3200(店頭), 3200-4000(店外)
        float viewStart = cameraX;
        float viewEnd = cameraX + 1280.0f;

        // 各エリアを描画
        struct AreaInfo {
            float start, end;
            int handle;
        };

        AreaInfo areas[] = {
            {0.0f, 800.0f, bg_backdoor},
            {800.0f, 1600.0f, bg_storeinner},
            {1600.0f, 2400.0f, bg_cashregister},
            {2400.0f, 3200.0f, bg_storefront},
            {3200.0f, 8000.0f, bg_outside}
        };

        for (const auto& area : areas) {
            // 画面内に表示されるエリアのみ描画
            if (area.end > viewStart && area.start < viewEnd) {
                float areaDrawStart = area.start - cameraX;

                if (area.handle != -1) {
                    DrawExtendGraph((int)areaDrawStart, 0,
                                   (int)(areaDrawStart + (area.end - area.start)), 720,
                                   area.handle, TRUE);
                }
                else {
                    // 画像がない場合はグラデーション
                    DrawBox((int)areaDrawStart, 0,
                           (int)(areaDrawStart + (area.end - area.start)), 720,
                           GetColor(30, 30, 50), TRUE);
                }
            }
        }

        if (floor) floor->Draw(cameraX);
        itemManager.Draw(cameraX);
        if (player1) player1->Draw(cameraX);

        // UI描画（最後に描画してデバッグ表示より前面に）
        DrawTimer();
        DrawPhaseInfo();

        // デバッグ情報（右下に表示）
        static bool showDebug = false;
        if (CheckHitKey(KEY_INPUT_F1)) {
            static int lastPress = 0;
            if (GetNowCount() - lastPress > 200) {
                showDebug = !showDebug;
                lastPress = GetNowCount();
            }
        }

        if (showDebug && player1) {
            DrawFormatString(950, 680, GetColor(255, 255, 0),
                "Debug: PlayerX=%.0f CameraX=%.0f [F1]非表示",
                player1->GetX(), cameraX);
        }
    }
    else if (currentPhase == GamePhase::Reasoning) {
        if (showResult) {
            DrawResult();
        }
        else {
            // 暗めの背景
            DrawBox(0, 0, 1280, 720, GetColor(20, 20, 30), TRUE);

            if (reasoningUI && reasoningManager) {
                reasoningUI->Draw(reasoningManager);
            }
        }
    }

    __super::Draw();
}

void InGameScene::Finalize()
{
    if (player1) {
        player1->StopAudio();
    }

    if (mainbgm >= 0) {
        StopSoundMem(mainbgm);
        DeleteSoundMem(mainbgm);
        mainbgm = -1;
    }

    // 背景画像削除
    if (bg_backdoor >= 0) {
        DeleteGraph(bg_backdoor);
        bg_backdoor = -1;
    }
    if (bg_storeinner >= 0) {
        DeleteGraph(bg_storeinner);
        bg_storeinner = -1;
    }
    if (bg_cashregister >= 0) {
        DeleteGraph(bg_cashregister);
        bg_cashregister = -1;
    }
    if (bg_storefront >= 0) {
        DeleteGraph(bg_storefront);
        bg_storefront = -1;
    }
    if (bg_outside >= 0) {
        DeleteGraph(bg_outside);
        bg_outside = -1;
    }

    delete player1;
    player1 = nullptr;

    delete floor;
    floor = nullptr;

    delete reasoningManager;
    reasoningManager = nullptr;

    delete reasoningUI;
    reasoningUI = nullptr;

    __super::Finalize();
}

eSceneType InGameScene::GetNowSceneType() const
{
    return eSceneType::eInGame;
}

void InGameScene::TransitionToReasoning()
{
    currentPhase = GamePhase::Reasoning;

    if (reasoningManager) {
        std::vector<std::string> collectedEvidence = itemManager.GetCollectedItems();

        reasoningManager->FilterOptions(collectedEvidence);
        reasoningManager->SetActive(true);

        if (reasoningUI) {
            reasoningUI->SetEvidenceList(collectedEvidence);
        }

        printfDx("推理フェーズ開始 - 収集証拠数: %d\n", (int)collectedEvidence.size());
    }
}

void InGameScene::DrawTimer() const
{
    int x = 20, y = 20;
    int boxWidth = 220, boxHeight = 70;

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 220);
    DrawBox(x - 10, y - 10, x + boxWidth, y + boxHeight, GetColor(0, 0, 0), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    unsigned int borderColor = GetColor(100, 100, 100);
    if (remainingTime < 10.0f) {
        borderColor = GetColor(255, 100, 100);
    }
    else if (remainingTime < 30.0f) {
        borderColor = GetColor(255, 200, 100);
    }

    DrawBox(x - 10, y - 10, x + boxWidth, y + boxHeight, borderColor, FALSE);

    int minutes = (int)(remainingTime / 60.0f);
    int seconds = (int)(remainingTime) % 60;

    unsigned int color = GetColor(255, 255, 255);
    if (remainingTime < 10.0f) {
        color = GetColor(255, 0, 0);
    }
    else if (remainingTime < 30.0f) {
        color = GetColor(255, 200, 0);
    }

    DrawFormatString(x, y, color, "Time: %02d:%02d", minutes, seconds);

    if (remainingTime < 10.0f && remainingTime > 0.0f) {
        static float blinkTimer = 0.0f;
        blinkTimer += 0.1f;
        if ((int)(blinkTimer * 5) % 2 == 0) {
            DrawFormatString(x, y + 30, GetColor(255, 100, 100), "急げ!");
        }
    }
    else if (remainingTime < 30.0f && remainingTime > 0.0f) {
        DrawFormatString(x, y + 30, GetColor(255, 200, 100), "あと少し...");
    }
}

void InGameScene::DrawPhaseInfo() const
{
    int x = 20, y = 100;
    int boxWidth = 220, boxHeight = 90;

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 220);
    DrawBox(x - 10, y - 10, x + boxWidth, y + boxHeight, GetColor(0, 0, 0), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    DrawBox(x - 10, y - 10, x + boxWidth, y + boxHeight, GetColor(100, 100, 100), FALSE);

    DrawFormatString(x, y, GetColor(255, 255, 255),
        "証拠: %d/%d",
        itemManager.GetCollectedCount(),
        itemManager.GetTotalCount());

    int barX = x;
    int barY = y + 25;
    int barWidth = 180;
    int barHeight = 12;

    DrawBox(barX, barY, barX + barWidth, barY + barHeight, GetColor(50, 50, 50), TRUE);

    float progress = (float)itemManager.GetCollectedCount() / (float)itemManager.GetTotalCount();
    int fillWidth = (int)(barWidth * progress);

    unsigned int barColor = GetColor(100, 200, 100);
    if (progress >= 1.0f) {
        barColor = GetColor(255, 215, 0);
    }

    DrawBox(barX, barY, barX + fillWidth, barY + barHeight, barColor, TRUE);

    DrawFormatString(x, y + 45, GetColor(200, 200, 200), "[E] 証拠リスト");
    DrawFormatString(x, y + 65, GetColor(150, 150, 150), "[ESC] タイトル");
}

void InGameScene::DrawOpening() const
{
    DrawBox(0, 0, 1280, 720, GetColor(10, 10, 20), TRUE);

    int x = 80, y = 50;
    int boxWidth = 1120, boxHeight = 620;

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 240);
    DrawBox(x - 10, y - 10, x + boxWidth, y + boxHeight, GetColor(0, 0, 0), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    DrawBox(x - 10, y - 10, x + boxWidth, y + boxHeight, GetColor(200, 180, 150), FALSE);
    DrawBox(x - 8, y - 8, x + boxWidth - 2, y + boxHeight - 2, GetColor(150, 130, 100), FALSE);

    SetFontSize(28);
    DrawFormatString(x + 300, y + 10, GetColor(255, 200, 100), "深夜の雑貨店殺人事件");
    SetFontSize(16);

    int currentY = y + 55;
    int lineHeight = 26;

    std::vector<std::string> lines = {
        "【時刻】午後11時30分",
        "【場所】桜町駅前商店街「ミナト雑貨店」",
        "",
        "深夜、終電を逃した天才探偵・神楽崎誠は",
        "静まり返った商店街を歩いて帰路についていた。",
        "",
        "そのとき――「助けて...！誰か...！」",
        "",
        "物音と悲鳴に気づき、雑貨店に駆け込むと",
        "そこには倒れた店主と、散乱した証拠品が...",
        "",
        "探偵：「この事件...1分あれば解決できます」",
        "警察：「面白い。やってみろ」",
        "",
        "【ミッション】",
        "・制限時間60秒で証拠を集めよ",
        "・←→で移動、Zで証拠を調べる",
        "・Eキーで証拠リストを表示",
        "",
        "[Z] または [A]ボタン でスタート",
        "[ESC] タイトルへ戻る"
    };

    for (const auto& line : lines) {
        unsigned int color = GetColor(255, 255, 255);

        if (line.find("【") != std::string::npos) {
            color = GetColor(255, 200, 100);
        }
        else if (line.find("探偵") != std::string::npos) {
            color = GetColor(100, 200, 255);
        }
        else if (line.find("警察") != std::string::npos) {
            color = GetColor(255, 100, 100);
        }
        else if (line.find("[Z]") != std::string::npos ||
            line.find("[ESC]") != std::string::npos) {
            color = GetColor(255, 255, 100);
        }

        DrawFormatString(x + 30, currentY, color, "%s", line.c_str());
        currentY += lineHeight;
    }
}

void InGameScene::DrawResult() const
{
    DrawBox(0, 0, 1280, 720, GetColor(0, 0, 0), TRUE);

    int x = 200, y = 150;
    int boxWidth = 880, boxHeight = 420;

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 240);
    DrawBox(x - 10, y - 10, x + boxWidth, y + boxHeight, GetColor(20, 20, 40), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    unsigned int borderColor = isCorrect ? GetColor(100, 255, 100) : GetColor(255, 100, 100);
    DrawBox(x - 10, y - 10, x + boxWidth, y + boxHeight, borderColor, FALSE);
    DrawBox(x - 8, y - 8, x + boxWidth - 2, y + boxHeight - 2, borderColor, FALSE);

    SetFontSize(48);
    if (isCorrect) {
        DrawFormatString(x + 300, y + 50, GetColor(255, 255, 100), "事件解決！");
    }
    else {
        DrawFormatString(x + 280, y + 50, GetColor(255, 100, 100), "推理失敗...");
    }
    SetFontSize(16);

    int currentY = y + 150;

    if (isCorrect) {
        DrawFormatString(x + 100, currentY, GetColor(255, 255, 255),
            "見事だ！1分で事件の真相を見抜いた！");
        currentY += 40;
        DrawFormatString(x + 100, currentY, GetColor(200, 200, 200),
            "犯人：木村達也（被害者の弟）");
        currentY += 30;
        DrawFormatString(x + 100, currentY, GetColor(200, 200, 200),
            "動機：遺産相続と借金返済");
        currentY += 30;
        DrawFormatString(x + 100, currentY, GetColor(200, 200, 200),
            "凶器：血のついた金属バット");
        currentY += 50;
        DrawFormatString(x + 150, currentY, GetColor(255, 215, 0),
            "完璧な推理だ！天才探偵の名に恥じない！");
    }
    else {
        DrawFormatString(x + 150, currentY, GetColor(255, 255, 255),
            "推理が外れてしまった...");
        currentY += 40;
        DrawFormatString(x + 100, currentY, GetColor(200, 200, 200),
            "証拠が足りなかったか、");
        currentY += 30;
        DrawFormatString(x + 100, currentY, GetColor(200, 200, 200),
            "それとも推理を誤ったか...");
        currentY += 50;
        DrawFormatString(x + 180, currentY, GetColor(150, 150, 150),
            "もう一度挑戦してみよう");
    }

    currentY += 80;
    DrawFormatString(x + 280, currentY, GetColor(255, 255, 100),
        "[Z] タイトルへ戻る");
}