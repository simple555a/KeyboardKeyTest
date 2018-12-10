#ifndef KEYBOARDHOOK_H
#define KEYBOARDHOOK_H
#include <QObject>
#include <windows.h>
#include <QThread>

LRESULT  CALLBACK KeyboardHookProc(int n_Code, WPARAM w_Param, LPARAM l_Param);

class KeyboardHook  : public QObject
{
    Q_OBJECT
public:
    KeyboardHook(QObject* parent = 0);
    ~KeyboardHook();

    static KeyboardHook * GetInstance()
    {
        if(m_pKeyboardHook == NULL)
        {
            m_pKeyboardHook = new KeyboardHook();
        }
        return m_pKeyboardHook;
    }
    //设置钩子占用开关
    bool SetHookOccupy(const bool &Switch);
    //获取钩子占用开关
    bool GetHookOccupy();
    //设置钩子信号锁
    bool SetHookInfoLock(const bool &b_Lock);
    //发送键盘按键信息
    void SendVkCode(int n_VkCode);
    //关闭钩子
    void CloseHook();
    //发送按键按下消息
    void SendKeyUp(int n_VkCode);
    //发送按键弹起消息
    void SendKeyDown(int n_VkCode);
    //初始化钩子
    void InitHook();
private:

signals:
    //键盘按键消息信号
    void sig_KeyboardHookMsg(int);
    //键盘按下信号
    void sig_KeyDown(int);
    //键盘弹起信号
    void sig_KeyUp(int);

private:
    static KeyboardHook *m_pKeyboardHook;
    QThread *m_pQThread;
    bool m_bOccupySwitch;   //消息占用开关
    bool m_bHookState;      //钩子状态
    bool m_bHoolInfoLock;   //信号锁
};

#endif // KEYBOARDHOOK_H
