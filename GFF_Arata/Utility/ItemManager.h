#pragma once
#include <vector>
#include <string>
#include <memory>
#include "../Objects/Item.h"
#include "InputManager.h"

enum class ItemMode {
    List,   // リスト表示
    Detail  // 詳細表示
};

class ItemManager {
private:
    std::vector<std::unique_ptr<Item>> items;
    bool isListOpen;  // アイテムリストが開いているか
    ItemMode mode;
    int selectedIndex;

    void DrawPixelArtBox(int x, int y, int width, int height) const;

public:
    ~ItemManager() = default;

    void Add(std::unique_ptr<Item> item);
    void Init();
    void Update(float playerX, float playerY, float deltaTime);
    void Draw(float cameraOffsetX = 0.0f) const;
    void ToggleList();  // アイテムリストの開閉

    // ゲッター
    std::vector<std::string> GetCollectedItems() const;
    int GetCollectedCount() const;
    int GetTotalCount() const { return (int)items.size(); }
    const std::vector<std::unique_ptr<Item>>& GetItems() const { return items; }

    const Item* GetSelectedItem() const;

    bool IsOpen() const { return isListOpen; }
};
