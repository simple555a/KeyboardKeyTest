#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle(QStringLiteral("键盘按键测试软件"));
    this->setFixedSize(this->size());

    m_pMainControl = NULL;
    m_pKeyboardShow = NULL;
    m_pQWidgetKeyboard = NULL;
    m_pQGridLayout = NULL;
    m_pDataAndInfoFile = NULL;

    m_pMainControl = new MainControl;
    m_pKeyboardShow = new KeyboardShow;
    m_pQGridLayout = new QGridLayout;
    m_pDataAndInfoFile = DataAndInfoFile::GetInstance();

    connect(m_pMainControl, SIGNAL(sig_Reset()), this, SLOT(slot_Reset()));
    connect(m_pMainControl, SIGNAL(sig_Msg(QString)), this, SLOT(slot_Msg(QString)));
    connect(m_pMainControl, SIGNAL(sig_WorkingState(WORKINGSTATE)), this, SLOT(slot_WorkingState(WORKINGSTATE)));


    connect(ui->pushButton_6HWired, &QPushButton::clicked, this, &MainWindow::Select_6H_Wired);
    connect(ui->pushButton_6HWireless, &QPushButton::clicked, this, &MainWindow::Select_6H_Wirless);
    connect(ui->pushButton_7HWired, &QPushButton::clicked, this, &MainWindow::Select_7P_Wired);
    connect(ui->pushButton_7HWireless, &QPushButton::clicked, this, &MainWindow::Select_7P_Wirless);

    connect(ui->pushButton_7SWired, &QPushButton::clicked, this, &MainWindow::Select_7S_Wired);
    connect(ui->pushButton_7RWired, &QPushButton::clicked, this, &MainWindow::Select_7R_Wired);

    connect(ui->pushButton_Return, &QPushButton::clicked, this, &MainWindow::Select_Return);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Select_6H_Wired()
{
    m_strType = "6H_Wired";
    m_pMainControl->PrepareCheck(WIRED, m_strType);
    CreatKeyboardShow();
    ui->stackedWidget->setCurrentIndex(1);
    m_pMainControl->LockKeyboard(true);
    m_pKeyboardShow->ConnectKeyInfo();
}

void MainWindow::Select_6H_Wirless()
{
    m_strType = "6H_Wirless";
    m_pMainControl->PrepareCheck(WIRLESS, m_strType);
    CreatKeyboardShow();
    ui->stackedWidget->setCurrentIndex(1);
    m_pMainControl->LockKeyboard(true);
    m_pKeyboardShow->ConnectKeyInfo();
}

void MainWindow::Select_7P_Wired()
{
    m_strType = "7P_Wired";
    m_pMainControl->PrepareCheck(WIRED, m_strType);
    CreatKeyboardShow();
    ui->stackedWidget->setCurrentIndex(1);
    m_pMainControl->LockKeyboard(true);
    m_pKeyboardShow->ConnectKeyInfo();
}

void MainWindow::Select_7P_Wirless()
{
    m_strType = "7P_Wirless";
    m_pMainControl->PrepareCheck(WIRLESS, m_strType);
    CreatKeyboardShow();
    ui->stackedWidget->setCurrentIndex(1);
    m_pMainControl->LockKeyboard(true);
    m_pKeyboardShow->ConnectKeyInfo();
}

void MainWindow::Select_7S_Wired()
{
    m_strType = "7S_Wired";
    m_pMainControl->PrepareCheck(WIRED, m_strType);
    CreatKeyboardShow();
    ui->stackedWidget->setCurrentIndex(1);
    m_pMainControl->LockKeyboard(true);
    m_pKeyboardShow->ConnectKeyInfo();
}

void MainWindow::Select_7R_Wired()
{
    m_strType = "7R_Wired";
    m_pMainControl->PrepareCheck(WIRED, m_strType);
    CreatKeyboardShow();
    ui->stackedWidget->setCurrentIndex(1);
    m_pMainControl->LockKeyboard(true);
    m_pKeyboardShow->ConnectKeyInfo();
}

void MainWindow::Select_Return()
{
    ui->stackedWidget->setCurrentIndex(0);

    if(m_pQWidgetKeyboard != NULL){
        m_pQGridLayout->removeWidget(m_pQWidgetKeyboard);

        delete m_pQWidgetKeyboard;
        m_pQWidgetKeyboard = NULL;
    }

    m_pMainControl->LockKeyboard(false);
    m_pKeyboardShow->DisconnectKeyInfo();
}

void MainWindow::CreatKeyboardShow()
{
    if(m_pQWidgetKeyboard != NULL){
        m_pQGridLayout->removeWidget(m_pQWidgetKeyboard);

        delete m_pQWidgetKeyboard;
        m_pQWidgetKeyboard = NULL;
    }

    m_pQWidgetKeyboard = m_pKeyboardShow->CreatKeyboardShow(m_strType);
    m_pQGridLayout->addWidget(m_pQWidgetKeyboard);

//    ui->stackedWidget->widget(1)

    if(ui->page_2->layout() == NULL)
        ui->page_2->setLayout(m_pQGridLayout);
}

void MainWindow::KeyCheckShow()
{
    QMap<int, QList<KEYBOARDRESULTSTATE>> map_KeyResultShow;
    KEYTESTFEASIBILITY Feasibility;

    bool ret = m_pMainControl->GetKeyResult(map_KeyResultShow, Feasibility);
    if(!ret){
        m_pKeyboardShow->ErrorKeyboard(QStringLiteral("无法测试"));
        return;
    }

    if(Feasibility == CANNOTTEST)    {
        m_pKeyboardShow->ErrorKeyboard(QStringLiteral("无法测试"));
        return;
    }

    m_pKeyboardShow->ShowKeyboardState(map_KeyResultShow);
}

void MainWindow::slot_Msg(QString str_Msg)
{

}

void MainWindow::slot_Reset()
{
    m_pKeyboardShow->ResetKeyboard("");
}

void MainWindow::slot_WorkingState(WORKINGSTATE State)
{
    switch (State) {
    case READY_S:
        ui->label_State->setStyleSheet("background:" + Color_ready);
        ui->label_Remide->setText(QStringLiteral("测试准备"));
        break;
    case RUN_S:
        ui->label_State->setStyleSheet("background:" + Color_run);
        ui->label_Remide->setText(QStringLiteral("测试运行"));
        break;
    case MATCH_NG_S:
        ui->label_State->setStyleSheet("background:" + Color_warning);
        ui->label_Remide->setText(QStringLiteral("对码失败"));
        m_pKeyboardShow->ErrorKeyboard(QStringLiteral("对码失败"));
        break;
    case MATCH_OK_S:
        ui->label_State->setStyleSheet("background:" + Color_run);
        ui->label_Remide->setText(QStringLiteral("对码成功"));
        break;
    case PASS_S:
        ui->label_State->setStyleSheet("background:" + Color_pass);
        ui->label_Remide->setText(QStringLiteral("测试通过"));
        KeyCheckShow();
        break;
    case ERROR_S:
        ui->label_State->setStyleSheet("background:" + Color_warning);
        ui->label_Remide->setText(QStringLiteral("测试失败"));
        KeyCheckShow();
        break;
    default:
        break;
    }
}
