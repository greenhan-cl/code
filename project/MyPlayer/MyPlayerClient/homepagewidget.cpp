#include "homepagewidget.h"
#include "ui_homepagewidget.h"
#include "util.h"
#include "videobox.h"

HomePageWidget::HomePageWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HomePageWidget)
{
    ui->setupUi(this);
    //标签和分类按钮
    initKindsAndTags();

    //connectSignalAndSlot();
    // 刷新和置顶按钮
    initRefreshAndTop();
    // 初始化视频列表
    initVideos();
}

HomePageWidget::~HomePageWidget()
{
    delete ui;
}

void HomePageWidget::initKindsAndTags()
{
    // 创建分类按钮
    QPushButton *kindBtn = buildSelectBtn(ui->classify, "#3ECEFF", "分类");
    ui->classifyHLayout->addWidget(kindBtn);
    //创建分类
    QList<QString> kinds = {"分享", "教程", "探店", "测评", "记录", "专题", "食材", "杂谈"};
    //创建分类按钮
    for (auto &kind : kinds)
    {
        QPushButton *kindBtn = buildSelectBtn(ui->classify, "#222222", kind);
        ui->classifyHLayout->addWidget(kindBtn);
        // 按钮点击后修改按钮上颜⾊
        connect(kindBtn, &QPushButton::clicked, this, [=](){
            onKindBtnClicked(kindBtn);
        });
    }

    ui->classifyHLayout->setSpacing(8);

    // 分类和该分类下所有标签映射
    tags = {
        {"分享", {"美食分享", "私藏推荐", "外卖分享", "回购清单"}},
        {"教程", {"家常菜", "川菜", "粤菜", "湘菜", "简减脂餐", "糕点"}},
        {"探店", {"餐厅探店", "路边小吃", "夜市", "老字号", "新店体验"}},
        {"测评", {"性价比", "外卖测评", "零食测评", "餐厅对比"}},
        {"记录", {"吃播", "一日三餐", "美食Vlog", "宵夜时间"}},
        {"专题", {"地方美食", "节日美食", "时令美食", "美食排行邦"}},
        {"食材", {"素食", "肉类", "海鲜", "时令蔬果", "半成品"}},
        {"杂谈", {"吃货感悟", "饮食习惯", "真香现场", "踩坑经历"}}};
    resetTags(tags["分享"]);
}

void HomePageWidget::initVideos()
{
    for(int i=0;i<16;++i){
        VideoBox* videoBox=new VideoBox();
        ui->videoGLayout->addWidget(videoBox,i/4,i%4);
    }
}



void HomePageWidget::initRefreshAndTop()
{
    // 刷新和置顶按钮
    QWidget* widget = new QWidget(this);
    widget->setFixedSize(42, 94);
    widget->setStyleSheet("QPushButton:hover{background-color:#666666}"
                          "QPushButton{"
                          "background-color : #DDDDDD;"
                          "border-radius : 21px;"
                          "border : none;}");
    QVBoxLayout* layout = new QVBoxLayout(widget);
    // 刷新按钮
    QPushButton* refreshBtn = new QPushButton(widget);
    refreshBtn->setFixedSize(42, 42);
    refreshBtn->setStyleSheet("border-image :url(:/images/homePage/shuaxin.png);");
    // 置顶按钮
    QPushButton* topBtn = new QPushButton(widget);
    topBtn->setFixedSize(42, 42);
    topBtn->setStyleSheet("border-image : url(:/images/homePage/zhiding.png)");
    layout->addWidget(refreshBtn);
    layout->addWidget(topBtn);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(10);
    widget->move(1278, 618);
    widget->show();
    // 给两个按钮绑定槽函数
    connect(refreshBtn, &QPushButton::clicked, this,
            &HomePageWidget::onRefreshBtnClicked);
    connect(topBtn, &QPushButton::clicked, this,
            &HomePageWidget::onTopBtnClicked);
    
    LOG()<<"按钮正常被调用";
}

QPushButton *HomePageWidget::buildSelectBtn(QWidget *parent, const QString &color, const QString &text)
{
    QPushButton* btn = new QPushButton(text, parent);
    btn->setMinimumHeight(26);
    btn->setFixedWidth(text.size()*16+18+18);
    btn->setStyleSheet("color:" + color);
    return btn;
}

void HomePageWidget::resetTags(const QList<QString> &tags)
{
    // 创建标签
    QPushButton* tag = buildSelectBtn(ui->labels, "#3ECEFF", "标签");
    ui->labelHLayout->addWidget(tag);
    // 创建具体标签按钮
    for(auto& tagText : tags){
        QPushButton* labelBtn = buildSelectBtn(ui->labels, "#666666", tagText);
        ui->labelHLayout->addWidget(labelBtn);
        connect(labelBtn, &QPushButton::clicked, this, [=](){
           onTagBtnClicked(labelBtn);
        });
    }
    ui->labelHLayout->setSpacing(4);
}

void HomePageWidget::onKindBtnClicked(QPushButton *clickedkindBtn)
{
    // 当前被点击的分类按钮设置颜⾊
    clickedkindBtn->setStyleSheet("background-color: #F1FDFF;"
                                  "color:#3ECEFF;");
    // 清除其他分类上的颜⾊
    QList<QPushButton*> kindBtns = ui->classify->findChildren<QPushButton*>();
    for(auto kindBtn : kindBtns){
        if(clickedkindBtn!= kindBtn){
            kindBtn->setStyleSheet("color : #222222");
        }
    }
    // 销毁之前的标签
    QList<QPushButton*> labels = ui->labels->findChildren<QPushButton*>();
    for(auto label : labels){
        ui->labelHLayout->removeWidget(label);
        delete label;
    }
    // 根据当前选中分类，重新添加标签
    resetTags(tags[clickedkindBtn->text()]);
}

void HomePageWidget::onTagBtnClicked(QPushButton *clickedLabelBtn)
{
    // 设置当前被点击标签按钮上颜⾊
    clickedLabelBtn->setStyleSheet("background-color: #F1FDFF;"
                                   "color:#3ECEFF;");
    // 清空之前的标签颜⾊
    QList<QPushButton*> tagBtns = ui->labels->findChildren<QPushButton*>();
    for(auto tagBtn : tagBtns){
        if(tagBtn != clickedLabelBtn){
            tagBtn->setStyleSheet("color : #666666");
        }
    }
}

void HomePageWidget::onRefreshBtnClicked()
{
    LOG()<<"刷新按钮点击";
}

void HomePageWidget::onTopBtnClicked()
{
    LOG()<<"置顶按钮点击";
}
