#include "data.h"

namespace model{

    int KindAndTag::id = 10000;

    KindAndTag::KindAndTag()
    {
        // 构建分类及其id
        QList<QString> kinds = {"分享", "教程", "探店", "测评", "记录", "专题", "食材", "杂谈"};
        for(auto& kind : kinds){
            kindIds.insert(kind,id++);
        }
    // 构建标签及其id
    QHash<QString, QList<QString>> kindsAndTags = {
       {"分享", {"美食分享", "私藏推荐", "外卖分享", "回购清单"}},
       {"教程", {"家常菜", "川菜", "粤菜", "湘菜", "简减脂餐", "糕点"}},
       {"探店", {"餐厅探店", "路边小吃", "夜市", "老字号", "新店体验"}},
       {"测评", {"性价比", "外卖测评", "零食测评", "餐厅对比"}},
       {"记录", {"吃播", "一日三餐", "美食Vlog", "宵夜时间"}},
       {"专题", {"地方美食", "节日美食", "时令美食", "美食排行邦"}},
       {"食材", {"素食", "肉类", "海鲜", "时令蔬果", "半成品"}},
       {"杂谈", {"吃货感悟", "饮食习惯", "真香现场", "踩坑经历"}}};

    // 构建分类下：各个标签及其对应id
    for(auto& kind : kinds){
        // 构建kind下所有标签及其id
        QList<QString>& tags = kindsAndTags[kind];
        QHash<QString, int> tagIdsOfKind;
        for(auto& tag : tags){
            tagIdsOfKind.insert(tag, id++);
        }
        tagIds.insert(kind, tagIdsOfKind);
    }
    }

    const QList<QString> KindAndTag::getAllKinds() const
    {
        return kindIds.keys();
    }

    const QHash<QString, int> KindAndTag::getTagsByKind(QString kind) const
    {
        return tagIds[kind];
    }

    int KindAndTag::getKindId(QString kind) const
    {
        return kindIds[kind];
    }

    int KindAndTag::getTagId(QString kind, QString tag) const
    {
        return tagIds[kind][tag];
    }



}//end model
