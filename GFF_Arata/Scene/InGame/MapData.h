#pragma once
#include <string>
#include <vector>
#include "../../Objects/Item.h"

// エリア定義
enum class MapArea {
    BackDoor,      // 裏口
    StoreInner,    // 店内奥
    CashRegister,  // レジ周辺
    StoreFront,    // 店頭
    Outside        // 店外
};

// 証拠データ構造体
struct EvidenceData {
    int id;
    std::string name;
    std::string description;
    float x;
    float y;
    MiniGameType gameType;
    MapArea area;
};

// 背景オブジェクト構造体
struct BackgroundObject {
    std::string type;
    float x;
    float y;
    float width;
    float height;
    std::string imagePath;
};

// マップデータクラス
class MapData {
public:
    static const int AREA_WIDTH = 800;
    static const int AREA_GAP = 2000;

    // 各エリアの開始X座標
    static const int AREA0_START = 0;
    static const int AREA1_START = AREA0_START + AREA_WIDTH + AREA_GAP; // 2800
    static const int AREA2_START = AREA1_START + AREA_WIDTH + AREA_GAP; // 5600
    static const int AREA3_START = AREA2_START + AREA_WIDTH + AREA_GAP; // 8400
    static const int AREA4_START = AREA3_START + AREA_WIDTH + AREA_GAP; // 11200

    // マップ全体の幅（計算用）
    static const int MAP_WIDTH = AREA4_START + AREA_WIDTH;

    static const int MAP_HEIGHT = 720;
    static const int FLOOR_Y = 600;

    static std::vector<EvidenceData> GetEvidenceList() {
        return {
            // --- エリア0: 裏口 (0 ~ 800) ---
            { 1, "血のついた金属バット", "凶器と思われる。指紋が付着している",
              AREA0_START + 200.0f, 500.0f, MiniGameType::QuickTimeEvent, MapArea::BackDoor },

              // --- エリア1: 店内奥 (2800 ~ 3600) ---
              { 2, "防犯カメラの映像", "男性のような人影が映っている,",
                AREA1_START + 150.0f, 500.0f, MiniGameType::PasswordCrack, MapArea::StoreInner },

              { 3, "店主の手帳", "「23:00 弟と話し合い」とメモされている",
                AREA1_START + 600.0f, 500.0f, MiniGameType::None, MapArea::StoreInner },

                // --- エリア2: レジ周辺 (5600 ~ 6400) ---
                { 4, "レジの記録", "23:20が最後の会計。現金は残っている",
                  AREA2_START + 200.0f, 480.0f, MiniGameType::QuickTimeEvent, MapArea::CashRegister },

                { 5, "散乱した商品", "激しい争いがあった証拠",
                  AREA2_START + 500.0f, 520.0f, MiniGameType::None, MapArea::CashRegister },

                  // --- エリア3: 店頭 (8400 ~ 9200) ---
                  { 6, "割れた窓ガラス", "内側から割られている。強盗偽装か？",
                    AREA3_START + 400.0f, 490.0f, MiniGameType::PasswordCrack, MapArea::StoreFront },

                    // --- エリア4: 店外 (11200 ~ 12000) ---
                    { 7, "目撃者の証言", "23:10頃、女性が裏口に入るのを目撃",
                      AREA4_START + 200.0f, 510.0f, MiniGameType::QuickTimeEvent, MapArea::Outside },

                    { 8, "タバコの吸い殻", "店の前に落ちていた。被害者は非喫煙者",
                      AREA4_START + 600.0f, 500.0f, MiniGameType::None, MapArea::Outside }
        };
    }

    // 背景オブジェクトを取得（座標更新）
    static std::vector<BackgroundObject> GetBackgroundObjects() {
        return {
          
        };
    }

    static float GetAreaStartX(int areaIndex) {
        switch (areaIndex) {
        case 0: return (float)AREA0_START;
        case 1: return (float)AREA1_START;
        case 2: return (float)AREA2_START;
        case 3: return (float)AREA3_START;
        case 4: return (float)AREA4_START;
        default: return 0.0f;
        }
    }

    static std::string GetAreaName(MapArea area) {
        switch (area) {
        case MapArea::BackDoor: return "裏口";
        case MapArea::StoreInner: return "店内奥";
        case MapArea::CashRegister: return "レジ周辺";
        case MapArea::StoreFront: return "店頭";
        case MapArea::Outside: return "店外";
        default: return "不明";
        }
    }

    static MapArea GetAreaFromX(float x) {
        if (x < AREA0_START + AREA_WIDTH + 100) return MapArea::BackDoor;
        if (x < AREA1_START + AREA_WIDTH + 100) return MapArea::StoreInner;
        if (x < AREA2_START + AREA_WIDTH + 100) return MapArea::CashRegister;
        if (x < AREA3_START + AREA_WIDTH + 100) return MapArea::StoreFront;
        return MapArea::Outside;
    }
};