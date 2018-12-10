#include "keyboardhook.h"
#include "QDebug"

HHOOK m_hKeyboard = NULL;

LRESULT CALLBACK KeyboardHookProc(int n_Code, WPARAM w_Param, LPARAM l_Param)
{
    LPKBDLLHOOKSTRUCT pKB = (LPKBDLLHOOKSTRUCT)l_Param;
    KeyboardHook *m_pStateKeyboardHook = KeyboardHook::GetInstance();
    BOOL bKeyToShield = FALSE;
    if(n_Code == HC_ACTION){
        switch(w_Param){
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:{
            int n_VkCode = (int)(pKB->vkCode);
            m_pStateKeyboardHook->SendKeyDown(n_VkCode);
            break;
        }
        case WM_KEYUP:{
        case WM_SYSKEYUP:
            int n_VkCode = (int)(pKB->vkCode);
            m_pStateKeyboardHook->SendVkCode(n_VkCode);
            m_pStateKeyboardHook->SendKeyUp(n_VkCode);
            break;
        }
        default:
            break;
        }
    }
    //将消息还给钩子链，不要影响别人
    if(!m_pStateKeyboardHook->GetHookOccupy())
        return(bKeyToShield?TRUE:CallNextHookEx(m_hKeyboard, n_Code, w_Param, l_Param));
    else
        return TRUE;
}
KeyboardHook*  KeyboardHook::m_pKeyboardHook = NULL;


KeyboardHook::KeyboardHook(QObject *parent)
    : QObject(parent)
{
    m_bOccupySwitch = false;
    m_bHookState = false;
    m_bHoolInfoLock = false;
    m_pQThread = NULL;
}

KeyboardHook::~KeyboardHook()
{
    CloseHook();
}

bool KeyboardHook::SetHookOccupy(const bool &Switch)
{
    m_bOccupySwitch = Switch;
    qDebug()<<"m_bOccupySwitch"<<m_bOccupySwitch;
    return true;
}

bool KeyboardHook::GetHookOccupy()
{
    return m_bOccupySwitch&&m_bHoolInfoLock;
}

bool KeyboardHook::SetHookInfoLock(const bool &b_Lock)
{
    m_bHoolInfoLock = b_Lock;
    return true;
}

void KeyboardHook::InitHook()
{
    if(!m_bHookState){
//        m_hKeyboard = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, GetModuleHandle(NULL), NULL);
    }
}

void KeyboardHook::CloseHook()
{
    if(m_pQThread != NULL){
        m_pQThread->quit();
        m_pQThread->wait();
        delete m_pQThread;
        m_pQThread = NULL;
    }

    if(m_bHookState){
        if(m_hKeyboard != NULL){
//            UnhookWindowsHookEx(m_hKeyboard);
            m_hKeyboard = NULL;
        }
    }
}

void KeyboardHook::SendKeyUp(int n_VkCode)
{
    emit sig_KeyUp(n_VkCode);
}

void KeyboardHook::SendKeyDown(int n_VkCode)
{
    emit sig_KeyDown(n_VkCode);
}

void KeyboardHook::SendVkCode(int n_VkCode)
{
    if(!m_bHoolInfoLock)
        return;

    emit sig_KeyboardHookMsg(n_VkCode);
}

