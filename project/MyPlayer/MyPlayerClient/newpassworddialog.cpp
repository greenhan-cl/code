#include "newpassworddialog.h"
#include "ui_newpassworddialog.h"

NewPasswordDialog::NewPasswordDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewPasswordDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    // 信号槽连接
    connect(ui->cancelBtn, &QPushButton::clicked, this,
            &NewPasswordDialog::close);
    // 编辑框输⼊结束信号和槽函数绑定
    connect(ui->passwordEdit1, &QLineEdit::editingFinished, this,
            &NewPasswordDialog::onEditingFinished);
    connect(ui->passwordEdit2, &QLineEdit::editingFinished, this,
            &NewPasswordDialog::onEditingFinished);
    // 提交按钮点击信号和槽函数绑定
    connect(ui->submitBtn, &QPushButton::clicked, this,
            &NewPasswordDialog::onSubmitBtnClicked);
}

NewPasswordDialog::~NewPasswordDialog()
{
    delete ui;
}

QString NewPasswordDialog::getNewPassword() const
{
    return this->newPassword;
}

void NewPasswordDialog::onSubmitBtnClicked()
{
    if (!checkPasswordEdit()) {
        return;
    }
    // 保存结果
    this->newPassword = ui->passwordEdit1->text();
    // 关闭窗⼝
    this->close();
}

void NewPasswordDialog::onEditingFinished()
{
    checkPasswordEdit();
}

bool NewPasswordDialog::checkPasswordEdit()
{
    if (ui->passwordEdit1->text().isEmpty() || ui->passwordEdit2->text().isEmpty()) {
        ui->messageLabel->setText("密码不能为空");
        return false;
    }
    QString message = passwordValid(ui->passwordEdit1->text());
    if (!message.isEmpty()) {
        ui->messageLabel->setText(message);
        return false;
    }
    message = passwordValid(ui->passwordEdit2->text());
    if (!message.isEmpty()) {
        ui->messageLabel->setText(message);
        return false;
    }
    if (ui->passwordEdit1->text() != ui->passwordEdit2->text()) {
        ui->messageLabel->setText("两次密码输⼊必须⼀致");
        return false;
    }
    ui->messageLabel->setText("");
    return true;
}

QString NewPasswordDialog::passwordValid(const QString &password)
{
    // 密码要求包含⼤写字⺟, ⼩写字⺟, 标点符号, 数字⾄少两种类型, ⻓度在 8-16 位之间.
    // 1. 判定⻓度
    if (password.length() < 8) {
    }
    if (password.length() > 16) {
        return QString("密码⻓度不能⼤于 16 位!");
    }
    // 2. 统计每种字符是否存在. 存在⽤ 1 表⽰, 不存在⽤ 0 表⽰.
    enum CharType {
        Upper,
        Lower,
        Punct,
        Digit
    };
    QMap<CharType, int> charTypeMap;
    for (int i = 0; i < password.length(); i++) {
        QChar c = password.at(i);
        if (c.isUpper()) {
            charTypeMap[Upper] = 1;
        } else if (c.isLower()) {
            charTypeMap[Lower] = 1;
        } else if (c.isPunct()) {
            charTypeMap[Punct] = 1;
        } else if (c.isDigit()) {
            charTypeMap[Digit] = 1;
        } else {
            // 如果当前字符不是上述四种, 直接返回 false.
            return QString("密码中只能包含数字字⺟和标点符号!");
        }
    }
    // 3. 验证⾄少存在两种类型.
    int totalCount = 0;
    for (auto it = charTypeMap.begin(); it != charTypeMap.end(); ++it) {
        totalCount += it.value();
    }
    if (totalCount < 2) {
        return QString("密码中必须包含⾄少两类字符!");
    }
    return QString();
}
