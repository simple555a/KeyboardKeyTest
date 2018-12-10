#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "maincontrol.h"
#include "keyboardshow.h"
#include <QGridLayout>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    //选择6H有线型号
    void Select_6H_Wired();
    //选择6H无线型号
    void Select_6H_Wirless();
    //选择7H型号
    void Select_7P_Wired();
    //选择7H型号
    void Select_7P_Wirless();
    //选择7S型号
    void Select_7S_Wired();
    //选择7R型号
    void Select_7R_Wired();
    //选择返回
    void Select_Return();
    //创建键盘显示
    void CreatKeyboardShow();
    //键盘显示
    void KeyCheckShow();

signals:

private slots:
    //底层信息
    void slot_Msg(QString str_Msg);
    //重置
    void slot_Reset();
    //工作状态
    void slot_WorkingState(WORKINGSTATE State);

private:
    Ui::MainWindow *ui;

    MainControl *m_pMainControl;
    KeyboardShow *m_pKeyboardShow;
    QWidget *m_pQWidgetKeyboard;
    QGridLayout *m_pQGridLayout;
    DataAndInfoFile *m_pDataAndInfoFile;

    QString m_strType;  //键盘型号
};

#endif // MAINWINDOW_H
