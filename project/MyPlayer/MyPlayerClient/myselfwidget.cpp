#include "myselfwidget.h"
#include "ui_myselfwidget.h"
#include "modifymyselfdialog.h"
#include "myplayer.h"
#include "videobox.h"
#include "util.h"
#include <QFileDialog>
#include <QByteArray>

MyselfWidget::MyselfWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MyselfWidget)
{
    ui->setupUi(this);
    initUI();
    //给头像按钮绑定槽函数
    // 头像按钮绑定槽函数
    connect(ui->avatarBtn, &AvatarButton::clicked, this,
            &MyselfWidget::uploadAvatarBtnClicked);
    // 设置按钮绑定槽函数
    connect(ui->settingBtn, &QPushButton::clicked, this,
            &MyselfWidget::settingBtnClicked);
    connect(ui->uploadVideoBtn, &QPushButton::clicked, this,
            &MyselfWidget::uploadViewBtnClicked);

}

MyselfWidget::~MyselfWidget()
{
    delete ui;
}

void MyselfWidget::initUI()
{
    ui->attentionBtn->hide();

#ifdef TEST_UI
    //显示视频
    for(int i=0;i<16;++i){
        VideoBox* videoBox=new VideoBox();
        ui->layout->addWidget(videoBox,i/4,i%4);
    }
#endif

}

void MyselfWidget::uploadAvatarBtnClicked()
{
    // 1. 弹出对话框, 选择⽂件
    QString fileName = QFileDialog::getOpenFileName(nullptr, "选择头像", "",
                                                    "Image Files (*.jpg*.png)");
        if (fileName.isEmpty()) {
        LOG() << "取消选择头像";
        return;
    }
    QByteArray fileData = loadFileToByteArray(fileName);
    if (fileData.isEmpty()) {
        LOG() << "头像⽂件读取失败";
        return;
    }
    ui->avatarBtn->setIcon(makeCircleIcon(fileData, ui->avatarBtn->width()/2));
}

void MyselfWidget::settingBtnClicked()
{
    // 弹出对话框设置⽤⼾信息
    ModifyMyselfDialog* dialog = new ModifyMyselfDialog();
    dialog->exec();
    delete dialog;
}

void MyselfWidget::uploadViewBtnClicked()
{
    // 1. 弹出打开⽂件对话框，让⽤⼾选择要上传的视频⽂件
    QString videoFilePath = QFileDialog::getOpenFileName(nullptr, "上传视频",
                                                         "",
                                                         "Videos (*.mp4 *.rmvb *.avi*.mov)");
        if(!videoFilePath.isEmpty()){
        // 视频⼤⼩限制，上限为4G
        QFileInfo fileInfo(videoFilePath);
        int64_t fileSize = fileInfo.size();
        if(fileSize > 4*1024*1024){
            LOG()<<"视频⽂件必须⼩于4G";
            return;
        }
        emit switchUploadVideoPage(UploadPage);
    }
}
