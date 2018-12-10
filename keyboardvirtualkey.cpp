#include "keyboardvirtualkey.h"
#include <windows.h>

KeyboardVirtualKey::KeyboardVirtualKey(QObject *parent)
{

}

KeyboardVirtualKey::~KeyboardVirtualKey()
{

}

void KeyboardVirtualKey::SetVirtualKey(uint16_t us_Key)
{
    m_usKeyData = us_Key;
}

void KeyboardVirtualKey::FunctionKeyLock(uint16_t us_Key)
{
    PressKeyboard(us_Key);
}

void KeyboardVirtualKey::PressKeyboard(uint16_t us_Key)
{
//    INPUT input[4];
//    memset(input, 0, sizeof(input));
//    //设置模拟键盘输入
//    input[0].type = input[1].type = input[2].type = input[3].type = INPUT_KEYBOARD;
//    input[0].ki.wVk = input[2].ki.wVk = us_Key;
//    input[1].ki.wVk = input[3].ki.wVk = 0;
//    // 释放按键，这非常重要
//    input[2].ki.dwFlags = input[3].ki.dwFlags = KEYEVENTF_KEYUP;
//    SendInput(4,input,sizeof(INPUT));


    INPUT input[2];
    memset(input, 0, sizeof(input));
    //设置模拟键盘输入
    input[0].type = input[1].type = INPUT_KEYBOARD;
    input[0].ki.wVk = input[1].ki.wVk = us_Key;
    // 释放按键，这非常重要
    input[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2,input,sizeof(INPUT));
}

void KeyboardVirtualKey::run()
{
    FunctionKeyLock(m_usKeyData);
}
