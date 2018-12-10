#ifndef KEYBOARDSHOW_H
#define KEYBOARDSHOW_H
#include <QWidget>
#include "keyboardpushbutton.h"
#include "golab_define.h"
#include <QMap>
#include <QThread>
#include "filtervkcode.h"

class KeyboardShow : public QWidget
{
    Q_OBJECT
public:
    KeyboardShow(QWidget *parent = 0);
    ~KeyboardShow();

    //接收按键信息
    void ConnectKeyInfo();
    //屏蔽按键信息
    void DisconnectKeyInfo();
    //创建键盘窗口
    QWidget* CreatKeyboardShow(const QString &str_Type);
    //创建按键
    KeyboardPushButton* CreatPushButton(const STRUCT_KEYBOARDINFO &struct_RectInfo);
    //重置键盘显示
    void ResetKeyboard(QString str_Msg);
    //键盘按下
    void PressKeyboard(QString str_Msg);
    //键盘弹起
    void ReleaseKeyboard(QString str_Msg);
    //键盘错误
    void ErrorKeyboard(QString str_Msg);
    //显示键盘状态，按键次数校验（暂不用）
    bool ShowKeyboardState(const QMap<int,bool> &map_KeyboardCheck);
    //显示键盘状态，按键排序校验
    void ShowKeyboardState(QMap<int, QList<KEYBOARDRESULTSTATE> > map_KeyResultCheck);
    //单个按键按下
    void SingleKeyDown(int n_VkCode);
    //单个按键弹起
    void SingleKeyUp(int n_VkCode);

private:
    //键盘状态
    void StateKeyboard(const KEYBOARDSTATE &state,  const QString &str_Msg);

private slots:
    void slot_SingleKeyDown(int n_VkCode);
    void slot_SingleKeyUp(int n_VkCode);

private:
    QMap<int, KeyboardPushButton * > m_mapButtonInfo;
    QThread *m_pQThread;
    FilterVkCode *m_pFilterVkCode;      //过滤钩子类指针
};

#endif // KEYBOARDSHOW_H
