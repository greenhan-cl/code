#include "datacenter.h"

namespace model{

    DataCenter* DataCenter::getInstance()
    {
        //C++11
        static DataCenter* instance;
        return instance;
    }

    const KindAndTag *DataCenter::getKindAndTagsClassPtr()
    {
        if(nullptr == kindsAndTags){
            // 实例化kindsAndTags
            kindsAndTags = new KindAndTag();
        }
        return kindsAndTags;
    }

    DataCenter::DataCenter(QWidget *parent)
        : QWidget{parent}
    {

    }

}//end model


