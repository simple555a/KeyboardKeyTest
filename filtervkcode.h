#ifndef FILTERVKCODE_H
#define FILTERVKCODE_H
#include <QObject>
#include "keyboardhook.h"
#include "keyboardvirtualkey.h"

class FilterVkCode : public QObject
{
    Q_OBJECT
private:
    FilterVkCode(QObject* parent = 0);

public:
    static FilterVkCode * GetInstance(){
        if(m_pFilterVkCode == NULL){
            m_pFilterVkCode = new FilterVkCode();
        }
        return m_pFilterVkCode;
    }
    //初始化过滤钩子
    void InitFilterHook();
    //关闭过滤钩子
    void CloseFilterHook();
    //设置过滤钩子消息占用
    bool SetFilterHookOccupy(const bool &Switch);
    //过去过滤钩子消息占用
    bool GetFilterHookOccupy();
    //设置过滤钩子信号锁
    bool SetFilterHookInfoLock(const bool &b_Lock);

private:
    //键盘信息过滤器
    void KeyFilter(const int &n_VkCode, bool &b_Pass);
    //键盘信息回车键过滤
    void Filter_Enter(int &n_VkCode);
    //键盘信息Insert键过滤器
    void Filter_Insert(int &n_VkCode);

signals:
    //已过滤的键盘消息
    void sig_FilterHookMsg(int);
    //按键过滤后按下的消息
    void sig_FilterKeyDown(int);
    //键盘过滤后弹起的消息
    void sig_FilterKeyUp(int);

public slots:
    //钩子信息
    void slot_HookMsg(int n_VkCode);
    //键盘按下信息
    void slot_KeyDown(int n_VkCode);
    //键盘弹起信息
    void slot_KeyUp(int n_VkCode);

private:
    static FilterVkCode *m_pFilterVkCode;       //本类静态指针
    KeyboardHook *m_pKeyboardHook;              //键盘钩子类指针
    KeyboardVirtualKey *m_pKeyboardVirtualKey;  //键盘虚拟按键类指针

    bool m_bIS_OtherVK_RETURN;                  //回车键识别
    bool m_bHookState;                          //钩子状态
    bool m_bHoolInfoLock;                       //钩子休息锁状态

    bool m_bFunLock;                            //键盘lock键识别
    bool m_bIS_OtherVK_Fn;                      //键盘Fn识别
};

#endif // FILTERVKCODE_H
