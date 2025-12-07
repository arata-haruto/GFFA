#include "Suspect.h"

Suspect::Suspect(float posX, float posY, SuspectType type)
    : NPC(posX, posY, "", "容疑者"),
    suspectType(type),
    isGuilty(false)
{
    // 容疑者ごとの設定
    switch (type) {
    case SuspectType::Fujisaki:
        name = "藤崎美咲";
        role = "元従業員";
        state = NPCState::Cooperative;
        break;

    case SuspectType::Sasaki:
        name = "佐々木健太";
        role = "隣の店主";
        state = NPCState::Suspicious;
        break;

    case SuspectType::Kimura:
        name = "木村達也";
        role = "被害者の弟";
        state = NPCState::Suspicious;
        isGuilty = true;  // 真犯人
        break;

    case SuspectType::Yamada:
        name = "山田一郎";
        role = "常連客";
        state = NPCState::Cooperative;
        break;
    }
}

void Suspect::Initialize() {
    // 画像があればここで読み込む
    // handle = LoadGraph("...");

    SetupTestimonies();
}

void Suspect::SetupTestimonies() {
    testimonies.clear();

    switch (suspectType) {
    case SuspectType::Fujisaki:
        // 藤崎美咲の証言
        AddTestimony({ "私は置き忘れた私物を取りに来ただけです！", false, false, "" });
        AddTestimony({ "確かに1ヶ月前に解雇されましたが、\n店主を恨んでなんていません...", false, false, "" });
        AddTestimony({ "裏口の鍵？返すの忘れてたんです。\nでも何もしてません！", true, false, "" });
        AddTestimony({ "23:10頃に来て、私物を取ってすぐ帰りました。\nその時は店主は元気でしたよ。", true, false, "" });
        break;

    case SuspectType::Sasaki:
        // 佐々木健太の証言
        AddTestimony({ "大きな音が聞こえたから見に来たんだ。\nそしたらもう...", false, false, "" });
        AddTestimony({ "確かに最近は客を取られて困ってたが、\n殺すなんて考えたこともない！", false, false, "" });
        AddTestimony({ "23:20頃かな、ガラスが割れる音がして\n駆けつけたんだ。", true, false, "" });
        AddTestimony({ "俺は自分の店で在庫整理してた。\n一人だったから証明できないけど...", true, false, "" });
        break;

    case SuspectType::Kimura:
        // 木村達也の証言（犯人・矛盾あり）
        AddTestimony({ "兄から呼び出されて来たんです。\nまさかこんなことになるなんて...", false, false, "" });
        AddTestimony({ "23:00に来て、経営のことで少し話しました。\nでもすぐ帰ろうとしたんです。", true, false, "" });
        AddTestimony({ "借金？...ええ、ありますよ。\nでも兄を殺す理由にはなりません！", true, false, "" });
        // 矛盾する証言
        AddTestimony({ "裏口から？いや、私は表から入りましたよ。\n...なぜそんなことを？", true, true, "防犯カメラの映像" });
        AddTestimony({ "血のついたバット？知りません！\n私は何も...", true, true, "血のついた金属バット" });
        break;

    case SuspectType::Yamada:
        // 山田一郎の証言
        AddTestimony({ "いつものタバコを買いに来ただけだよ。", false, false, "" });
        AddTestimony({ "確かに最近、政治の話で店主と\n口論したことはある。", false, false, "" });
        AddTestimony({ "23:00頃にタバコを買って、\nすぐ帰ったんだ。", true, false, "" });
        AddTestimony({ "外でタバコを吸ってたから、\n吸い殻は私のだ。でもそれだけだ。", true, true, "タバコの吸い殻" });
        break;
    }
}