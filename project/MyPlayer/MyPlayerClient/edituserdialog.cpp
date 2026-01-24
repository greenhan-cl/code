#include "edituserdialog.h"
#include "ui_EditUserDialog.h"
#include "myplayer.h"
#include <QPushButton>

EditUserDialog::EditUserDialog(QWidget *parent ,const QString& text) :
    QDialog(parent),
    ui(new Ui::EditUserDialog)
{
    ui->setupUi(this);
    //去除窗口边框
    setWindowFlag(Qt::FramelessWindowHint);
    //设置窗口透明
    setAttribute(Qt::WA_TranslucentBackground);
    //设置窗口描述
    ui->titleLabel->setText(text);
    //将窗口移动到与主窗口左上角重合
    MyPlayer* myPlayer = MyPlayer::getInstance();
    this->move(myPlayer->mapToGlobal(QPoint(0,0)));

    // 对⼿机号编辑框需进⾏限制：以1开始的11位整形数字
    QRegularExpression regExp("^1\\d{10}$");
    QValidator* validator = new QRegularExpressionValidator(regExp, this);
    ui->phoneEdit->setValidator(validator);

    // 设置⽤⼾⻆⾊
    ui->roleComboBox->addItem("平台管理员");
    ui->roleComboBox->setCurrentIndex(0);

    // 取消按钮点击
    connect(ui->cancelBtn, &QPushButton::clicked, this,
            &EditUserDialog::onCancelBtnClicked);
    // 编辑框内容变化信号
    connect(ui->commentTextEdit, &QPlainTextEdit::textChanged, this, [=]{
        QString text = ui->commentTextEdit->toPlainText();
        int wordCount = text.size();
        if(wordCount <= 10){
            ui->wordCount->setText(QString::number(wordCount) + "/10");
        }else{
            ui->commentTextEdit->setPlainText(text.mid(0, 10));
        }
    });
}



EditUserDialog::~EditUserDialog()
{
    delete ui;

}

void EditUserDialog::onCancelBtnClicked()
{
    close();
}

