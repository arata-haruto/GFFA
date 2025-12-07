#pragma once
#include <string>
#include <vector>
#include "../../Objects/Item.h" // MiniGameTypeのためにインクルード

// エリア定義
enum class MapArea {
    BackDoor,      // 裏口 (0-800px)
    StoreInner,    // 店内奥 (800-1600px)
    CashRegister,  // レジ周辺 (1600-2400px)
    StoreFront,    // 店頭 (2400-3200px)
    Outside        // 店外 (3200-4000px)
};

// 証拠データ構造体
struct EvidenceData {
    int id;                          // 証拠ID
    std::string name;                // 証拠名
    std::string description;         // 説明
    float x;                         // X座標
    float y;                         // Y座標
    MiniGameType gameType;           // ミニゲームタイプ
    MapArea area;                    // 所属エリア
};

// 背景オブジェクト構造体
struct BackgroundObject {
    std::string type;                // タイプ（wall, shelf, counter, window）
    float x;                         // X座標
    float y;                         // Y座標
    float width;                     // 幅
    float height;                    // 高さ
    std::string imagePath;           // 画像パス（オプション）
};

// マップデータクラス
class MapData {
public:
    // マップサイズ定数
    static const int MAP_WIDTH = 4000;
    static const int MAP_HEIGHT = 720;
    static const int FLOOR_Y = 600;
    static const int FLOOR_HEIGHT = 120;

    // エリア境界
    static const int AREA1_END = 800;
    static const int AREA2_END = 1600;
    static const int AREA3_END = 2400;
    static const int AREA4_END = 3200;

    // 証拠データを取得
    static std::vector<EvidenceData> GetEvidenceList() {
        return {
            // 証拠① - 血のついた金属バット
            { 1, "血のついた金属バット", "凶器と思われる。指紋が付着している", 400.0f, 500.0f, MiniGameType::QuickTimeEvent, MapArea::BackDoor },

            // 証拠② - 防犯カメラの映像
            { 2, "防犯カメラの映像", "23:15に裏口から入る人影が映っている", 900.0f, 500.0f, MiniGameType::PasswordCrack, MapArea::StoreInner },

            // 証拠③ - 店主の手帳
            { 3, "店主の手帳", "「23:00 弟と話し合い」とメモされている", 1300.0f, 500.0f, MiniGameType::None, MapArea::StoreInner },

            // 証拠④ - レジの記録
            { 4, "レジの記録", "23:20が最後の会計。現金は残っている", 1800.0f, 480.0f, MiniGameType::QuickTimeEvent, MapArea::CashRegister },

            // 証拠⑤ - 散乱した商品
            { 5, "散乱した商品", "激しい争いがあった証拠", 2100.0f, 520.0f, MiniGameType::None, MapArea::CashRegister },

            // 証拠⑥ - 割れた窓ガラス
            { 6, "割れた窓ガラス", "内側から割られている。強盗偽装か？", 2600.0f, 490.0f, MiniGameType::PasswordCrack, MapArea::StoreFront },

            // 証拠⑦ - 目撃者の証言
            { 7, "目撃者の証言", "23:10頃、女性が裏口に入るのを目撃", 3300.0f, 510.0f, MiniGameType::QuickTimeEvent, MapArea::Outside },

            // 証拠⑧ - タバコの吸い殻
            { 8, "タバコの吸い殻", "店の前に落ちていた。被害者は非喫煙者", 3700.0f, 500.0f, MiniGameType::None, MapArea::Outside }
        };
    }

    // 背景オブジェクトを取得
    static std::vector<BackgroundObject> GetBackgroundObjects() {
        return {
            // 裏口エリア
            {"wall", 50.0f, 300.0f, 10.0f, 300.0f, ""},
            {"shelf", 200.0f, 450.0f, 100.0f, 150.0f, "Resource/Objects/Shelf.png"},
            {"shelf", 600.0f, 450.0f, 100.0f, 150.0f, "Resource/Objects/Shelf.png"},

            // 店内奥エリア
            {"shelf", 850.0f, 450.0f, 100.0f, 150.0f, "Resource/Objects/Shelf.png"},
            {"shelf", 1000.0f, 450.0f, 100.0f, 150.0f, "Resource/Objects/Shelf.png"},
            {"shelf", 1400.0f, 450.0f, 100.0f, 150.0f, "Resource/Objects/Shelf.png"},

            // レジ周辺エリア
            {"counter", 1700.0f, 500.0f, 200.0f, 100.0f, "Resource/Objects/Counter.png"},
            {"shelf", 2000.0f, 450.0f, 100.0f, 150.0f, "Resource/Objects/Shelf.png"},
            {"shelf", 2200.0f, 450.0f, 100.0f, 150.0f, "Resource/Objects/Shelf.png"},

            // 店頭エリア
            {"window", 2500.0f, 220.0f, 80.0f, 80.0f, "Resource/Objects/Window.png"},
            {"window", 2700.0f, 220.0f, 80.0f, 80.0f, "Resource/Objects/Window.png"},
            {"counter", 2900.0f, 500.0f, 200.0f, 100.0f, "Resource/Objects/Counter.png"},

            // 店外エリア
            {"wall", 3300.0f, 200.0f, 10.0f, 400.0f, ""}
        };
    }

    // エリア名を取得
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

    // X座標からエリアを判定
    static MapArea GetAreaFromX(float x) {
        if (x < AREA1_END) return MapArea::BackDoor;
        if (x < AREA2_END) return MapArea::StoreInner;
        if (x < AREA3_END) return MapArea::CashRegister;
        if (x < AREA4_END) return MapArea::StoreFront;
        return MapArea::Outside;
    }
};