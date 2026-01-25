#ifndef DATA_H
#define DATA_H

#include <QList>
#include <QString>
#include <QHash>
#include <QJsonObject>

// 该文件中，定义程序中各种数据对应的数据结构

namespace model{

    class KindAndTag
    {
    public:
        KindAndTag();
        // 获取所有分类
        const QList<QString> getAllKinds()const;
        // 获取⼀个分类下所有标签及id
        const QHash<QString, int> getTagsByKind(QString kind)const;
        // 获取分类的id
        int getKindId(QString kind)const;
        // 获取kind分类下包含的tag的id
        int getTagId(QString kind, QString tag)const;

    private:
        // key为分类名称，value为分类Id
        QHash<QString, int> kindIds;
        // 外层QHash的key为分类名称
        // 内层QHash的key为标签名称，value为标签Id
        QHash<QString, QHash<QString, int>> tagIds;
        static int id;

    };

}//end model



#endif // DATA_H
