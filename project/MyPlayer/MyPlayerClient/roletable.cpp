#include "roletable.h"
#include "ui_roletable.h"
#include "roletableitem.h"
#include "edituserdialog.h"
#include "util.h"

RoleTable::RoleTable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RoleTable)
{
    ui->setupUi(this);

    // 创建分⻚器并显⽰
    paginator = new Paginator(10, ui->PaginatorArea);
    paginator->move(0, 20);
    paginator->show();

    ui->userStatus->addItem("全部分类");
    ui->userStatus->addItem("启⽤");
    ui->userStatus->addItem("停⽤");
    ui->userStatus->setCurrentIndex(0);

    //更新表格
    updateRoleTable();
    // 限制编辑框只能输⼊⼿机号
    QRegularExpression regExp("^1\\d{10}$");
    QValidator* validator = new QRegularExpressionValidator(regExp, this);
    ui->phone->setValidator(validator);

    // 初始化重置和查询按钮的样式
    initStyleSheet();
    //绑定槽函数
    connect(ui->queryBtn,&QPushButton::clicked,
            this,&RoleTable::onQueryBtnClicked);
    connect(ui->resetBtn,&QPushButton::clicked,
            this,&RoleTable::onResetBtnClicked);
    connect(ui->insertBtn,&QPushButton::clicked,
            this,&RoleTable::onInsertBtnClicked);

}

RoleTable::~RoleTable()
{
    delete ui;
}

//初始化按钮
void RoleTable::initStyleSheet()
{
    styleSheet.insert("选中", "background-color:#3ECEFF;"
                              "border-radius:4px;"
                              "font-family:微软雅⿊;"
                              "font-size:14px;"
                              "color:#FFFFFF;");
    styleSheet.insert("未选中", "background:#FFFFFF;"
                                "border-radius:4px;"
                                "border:1px solid #DCDEE0;"
                                "font-family:微软雅⿊;"
                                "font-size:14px;"
                                "color:#222222;");
}

void RoleTable::updateRoleTable()
{
    // 添加RoleTableItem到表中
    for (int i = 0; i < 10; i++) {
        RoleTableItem* item = new RoleTableItem(this,i+1);
        ui->layout->addWidget(item);
    }
}

//新增用户槽函数
void RoleTable::onInsertBtnClicked()
{
    // 显⽰新增对后台⽤⼾对话框
    EditUserDialog* editUserDlg = new EditUserDialog(nullptr, "新增后台⽤⼾");
    editUserDlg->exec();
    delete editUserDlg;
}


//重置按钮槽函数
void RoleTable::onResetBtnClicked()
{
    // 设置按钮样式
    ui->resetBtn->setStyleSheet(styleSheet["选中"]);
    ui->queryBtn->setStyleSheet(styleSheet["未选中"]);
    ui->phone->setText("");
    ui->userStatus->setCurrentIndex(0);
    LOG()<<"重置按钮点击...";
}

//查询按钮槽函数
void RoleTable::onQueryBtnClicked()
{
    // 设置按钮样式
    ui->queryBtn->setStyleSheet(styleSheet["选中"]);
    ui->resetBtn->setStyleSheet(styleSheet["未选中"]);
    LOG()<<"查询按钮点击...";
}

