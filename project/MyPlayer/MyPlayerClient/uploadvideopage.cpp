#include "uploadvideopage.h"
#include "ui_uploadvideopage.h"
#include "./model/datacenter.h"
#include "util.h"
#include <QFileDialog>

UploadVideoPage::UploadVideoPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UploadVideoPage)
{
    ui->setupUi(this);

    // 获取所有分类，并更新到界⾯
    auto dataCenter = model::DataCenter::getInstance();
    auto kindAndTag = dataCenter->getKindAndTagsClassPtr();
    ui->kinds->addItems(kindAndTag->getAllKinds());
    ui->kinds->setCurrentIndex(-1); // 默认不选中
    // 提交按钮
    connect(ui->commitBtn, &QPushButton::clicked, this,
            &UploadVideoPage::onCommitBtnClicked);
    // QLineEdit::textChanged信号槽绑定
    connect(ui->videoTitle, &QLineEdit::textChanged, this,
            &UploadVideoPage::onLineEditTextChanged);
    // QPlainTextEdit::textChanged信号槽绑定
    connect(ui->plainTextEdit, &QPlainTextEdit::textChanged, this,
            &UploadVideoPage::onPlainEditTextChanged);
    // 更换图⽚按钮信号槽绑定
    connect(ui->changeButton, &QPushButton::clicked, this,
            &UploadVideoPage::onChangeBtnClicked);

}

UploadVideoPage::~UploadVideoPage()
{
    delete ui;
}

//提交槽函数
void UploadVideoPage::onCommitBtnClicked()
{
    // 视频上传成功，切换到我的⻚⾯
    emit switchMySelfPage(1);
}

//标题槽函数
void UploadVideoPage::onLineEditTextChanged(const QString &text)
{
    // 获取已经输⼊的⽂本
    QString leftWord = ui->leftWord->text();
    int wordCount = text.size();
    // 从界⾯截取出能容纳的总字数: 9/80, 截取到的就是90
    int linePos = leftWord.indexOf('/');
    QString totalWords = leftWord.mid(linePos+1);
    // 如果字数不够上限，继续输⼊
    if(totalWords.toInt() - wordCount >= 0){
        ui->leftWord->setText(QString::number(wordCount) + "/" + totalWords);
    }else{
        // TODO: 确认下是否需要提⽰
    }
}

//简介槽函数
void UploadVideoPage::onPlainEditTextChanged()
{
    // 获取已经输⼊的⽂本
    QString text = ui->plainTextEdit->toPlainText();
    int wordCount = text.size();
    // 从界⾯截取出能容纳的总字数: 27/1000, 截取到的就是27
    QString leftWord = ui->briefLeftWord->text();
    int linePos = leftWord.indexOf('/');
    QString totalWords = leftWord.mid(linePos+1);
    // 如果字数不够上限，继续输⼊
    if(totalWords.toInt() - wordCount >= 0){
        ui->briefLeftWord->setText(QString::number(wordCount) + "/" +
                                   totalWords);
    }else{
        // TODO: 确认下是否需要提⽰
        ui->plainTextEdit->setPlainText(text.mid(0, totalWords.toInt()));
        // 设置滚动条⼀直处于底部
        // textCursor: 获取当前光标的位置
        // movePosition: 移动光标位置，QTextCursor::End移动到⽂本的末尾
        // QTextCursor::MoveAnchor表⽰移动光标时，光标的位置是固定的，
        // 不会创建⼀个选区（即不会选中⽂本）
        QTextCursor textCursor = ui->plainTextEdit->textCursor();
        textCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        ui->plainTextEdit->setTextCursor(textCursor);
    }
}

void UploadVideoPage::onChangeBtnClicked()
{
    // 如果上传多个封⾯，使⽤最新的封⾯
    QString coverImagePath = QFileDialog::getOpenFileName(nullptr, "选择视频封⾯图", "","Images (*.png *.xpm *.jpg)");
        if(!coverImagePath.isEmpty()){
        // 将获取到的视频封⾯图显⽰到界⾯
        QPixmap pixmap(coverImagePath);
        pixmap = pixmap.scaled(ui->imageLabel->size(), Qt::IgnoreAspectRatio,
                               Qt::SmoothTransformation);
        ui->imageLabel->setPixmap(pixmap);
        repaint();
    }
}

// QComoBox中分类选择改变槽函数
void UploadVideoPage::onUpdateTags(const QString &kind)
{
    addTagsByKind(kind);
}

void UploadVideoPage::addTagsByKind(const QString &kind)
{
    // 1.添加之前先清空之前的标签
    QList<QPushButton*> tagBtnList = ui->tagWidget->findChildren<QPushButton*>();
    for(auto tagBtn : tagBtnList){
        // 删除标签按钮，提交按钮不能删除--
        ui->tagLayout->removeWidget(tagBtn);
        delete tagBtn;
    }
    // 将添加的弹簧删除掉
    // 弹簧本来是最后⼀个控件，当layoyut中的按钮删除之后，弹簧就变成第0个控件了
    QLayoutItem* spacerItem = ui->tagLayout->itemAt(ui->tagLayout->count()-1);
    ui->tagLayout->removeItem(spacerItem);
    // 2. 根据kind获取标签--注意：如果key不存在直接返回
    if(kind.isEmpty()){
        return;
    }
    auto kindAndTagPtr = model::DataCenter::getInstance()->getKindAndTagsClassPtr();
    auto kinds = model::DataCenter::getInstance()->getKindAndTagsClassPtr()->getAllKinds();
    auto tags = kindAndTagPtr->getTagsByKind(kind);
    // 3. 创建该标签对应的按钮
    for(auto& tag : tags.keys()){
        QPushButton* tagBtn = new QPushButton(ui->tagContent);
        tagBtn->setFixedSize(98, 49);
        tagBtn->setText(tag);
        tagBtn->setCheckable(true); // 设置按钮的状态为选中和未选中两种状态
        // QPushButton:checked 当按钮选中时
        // QPushButton:unchecked 当按钮未选中时
        tagBtn->setStyleSheet("QPushButton{"
                              "border : 1px solid #3ECEFE;"
                              "border-radius : 4px;"
                              "color : #3ECEFE;"
                              "font-family : 微软雅⿊;"
                              "font-size : 16px;"
                              "background-color : #FFFFFF;}"
                              "QPushButton:checked{"
                              "background-color : #3ECEFE;"
                              "color : #FFFFFF;}"
                              "QPushButton:unchecked{"
                              "background-color : #FFFFFF;"
                              "color : #3ECEFE;}");
        ui->tagLayout->addWidget(tagBtn);
    }
    // 在tagLayout最后放⼀个空⽩间距，将按钮挤到左侧
    ui->tagLayout->insertSpacing(tags.size(), ui->tagContent->width() - (98+20)*tags.size());
    ui->tagLayout->setSpacing(20);
}

