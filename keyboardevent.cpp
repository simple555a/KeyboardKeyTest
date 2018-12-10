#include "keyboardevent.h"
#include <QKeyEvent>
#include <windows.h>

KeyboardEvent::KeyboardEvent(QWidget *parent)
     : QWidget(parent)
{
    m_bEventSwitch = false;
    this->setFocus(Qt::ActiveWindowFocusReason);
}

void KeyboardEvent::SetEventSwitch(const bool &b_Switch)
{
    m_bEventSwitch = b_Switch;
}

bool KeyboardEvent::GetEventSwitch()
{
    return m_bEventSwitch;
}

void KeyboardEvent::keyPressEvent(QKeyEvent *event)
{
    if(!m_bEventSwitch)
        return;

    emit sig_PressNativeScanCode(event->nativeScanCode());
    emit sig_PressNativeVirtualKey(event->nativeVirtualKey());
    event->ignore();
}

void KeyboardEvent::keyReleaseEvent(QKeyEvent *event)
{
    if(!m_bEventSwitch)
        return;

    emit sig_ReleaseNativeScanCode(event->nativeScanCode());
    emit sig_ReleaseNativeVirtualKey(event->nativeVirtualKey());
    event->ignore();

    if(event->nativeVirtualKey() == 0x5b){
        INPUT input[4];
        memset(input, 0, sizeof(input));
        //设置模拟键盘输入
        input[0].type = input[1].type = input[2].type = input[3].type = INPUT_KEYBOARD;
        input[0].ki.wVk = input[2].ki.wVk = VK_LWIN;
        input[1].ki.wVk = input[3].ki.wVk = 0;
        // 释放按键，这非常重要
        input[2].ki.dwFlags = input[3].ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(4,input,sizeof(INPUT));
    }
}
