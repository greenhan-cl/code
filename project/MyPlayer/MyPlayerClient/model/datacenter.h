#ifndef DATACENTER_H
#define DATACENTER_H

#include <QWidget>
#include "data.h"

namespace model{

    class DataCenter : public QWidget
    {
        Q_OBJECT
    public:
        //单例
        static DataCenter* getInstance();
        // 获取所有分类
        const KindAndTag* getKindAndTagsClassPtr();

    private:
        explicit DataCenter(QWidget *parent = nullptr);

    signals:

    private:
        //分类和标签实例化指针
        KindAndTag* kindsAndTags = nullptr;


    };


}//end model



#endif // DATACENTER_H
