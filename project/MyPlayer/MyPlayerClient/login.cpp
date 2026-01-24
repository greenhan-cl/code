#include "login.h"
#include "ui_Login.h"
#include <QGraphicsDropShadowEffect>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);

    // 去除窗⼝边框，并设置窗⼝为模态对话框
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_ShowModal, true);
    // 窗⼝加上阴影效果
    setAttribute(Qt::WA_TranslucentBackground); // 阴影效果必须要窗⼝透明
    QGraphicsDropShadowEffect* shadowEffect = new
        QGraphicsDropShadowEffect(this);
    shadowEffect->setColor(Qt::black);
    shadowEffect->setBlurRadius(5);
    shadowEffect->setOffset(0);
    ui->loginBg->setGraphicsEffect(shadowEffect);
    // 密码登录 和 短信登录，假设默认为密码登录
    ui->messageWidget->show();
    ui->passwordWidget->hide();


    // lineEdit限制
    ui->passwordNum->setEchoMode(QLineEdit::Password);
    // ⼿机编辑框和账号限制
    QRegularExpression regExp("^1\\d{10}$");
    QValidator* validator = new QRegularExpressionValidator(regExp, this);
    ui->accountNum->setValidator(validator);
    ui->phoneNum->setValidator(validator);
    // 创建整数验证器，输⼊为0~999999
    QIntValidator* intValid = new QIntValidator(0, 999999, ui->authcode);
    ui->authcode->setValidator(intValid);
    ui->authcode->setMaxLength(6);
    // 最⼩化和关闭按钮槽函数
    connect(ui->min, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(ui->quit, &QPushButton::clicked, this, &QWidget::close);
    // 切换账号密码登录
    connect(ui->passwordBtn, &QPushButton::clicked, this,
            &Login::onPasswordBtnClicked);
    // 切换短信登录
    connect(ui->messageBtn, &QPushButton::clicked, this,
            &Login::onMessageBtnClicked);
}

Login::~Login()
{
    delete ui;
}

void Login::onPasswordBtnClicked()
{
    ui->messageWidget->hide();
    ui->passwordWidget->show();
    ui->passwordBtn->setStyleSheet("QPushButton{"
                                   "color : #3ECEFE;"
                                   "font-weight:bold;"
                                   "border : none;"
                                   "border-bottom : 6px solid #3ECEFE;}");
    ui->messageBtn->setStyleSheet("QPushButton{"
                                  "color : #222222;"
                                  "border : none;"
                                  "border-bottom: 2px solid #B5ECFF;}");
}

void Login::onMessageBtnClicked()
{
    ui->messageBtn->setStyleSheet("QPushButton{"
                                  "color : #3ECEFE;"
                                  "font-weight:bold;"
                                  "border : none;"
                                  "border-bottom : 6px solid #3ECEFE;}");
    ui->passwordBtn->setStyleSheet("QPushButton{"
                                   "color : #222222;"
                                   "border : none;"
                                   "border-bottom: 2px solid #B5ECFF;}");
    ui->messageWidget->show();
    ui->passwordWidget->hide();
}

