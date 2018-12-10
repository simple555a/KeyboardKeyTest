#include "filtervkcode.h"
#include <windows.h>
#include <QDebug>

FilterVkCode*  FilterVkCode::m_pFilterVkCode = NULL;

FilterVkCode::FilterVkCode(QObject *parent)
    : QObject(parent)
{
    m_bIS_OtherVK_RETURN = false;
    m_bIS_OtherVK_Fn = false;
    m_bHookState = false;
    m_bHoolInfoLock = false;

    m_bFunLock = false;

    m_pKeyboardHook = NULL;
    m_pKeyboardVirtualKey = NULL;

    m_pKeyboardHook = KeyboardHook::GetInstance();

    m_pKeyboardVirtualKey = new KeyboardVirtualKey;

    connect(m_pKeyboardHook, SIGNAL(sig_KeyboardHookMsg(int)), this, SLOT(slot_HookMsg(int)));
    connect(m_pKeyboardHook, SIGNAL(sig_KeyDown(int)), this, SLOT(slot_KeyDown(int)));
    connect(m_pKeyboardHook, SIGNAL(sig_KeyUp(int)), this, SLOT(slot_KeyUp(int)));
}

void FilterVkCode::InitFilterHook()
{
    m_pKeyboardHook->InitHook();
}

void FilterVkCode::CloseFilterHook()
{
    m_pKeyboardHook->CloseHook();
}

bool FilterVkCode::SetFilterHookOccupy(const bool &Switch)
{
    return m_pKeyboardHook->SetHookOccupy(Switch);
}

bool FilterVkCode::GetFilterHookOccupy()
{
    return m_pKeyboardHook->GetHookOccupy();
}

bool FilterVkCode::SetFilterHookInfoLock(const bool &b_Lock)
{
     return m_pKeyboardHook->SetHookInfoLock(b_Lock);
}

void FilterVkCode::KeyFilter(const int &n_VkCode, bool &b_Pass)
{
    qDebug()<<"KeyFilter"<<n_VkCode;
    qDebug()<<"m_bFunLock"<<m_bFunLock;
    if(!m_bFunLock){
        switch (n_VkCode) {
        case VK_NUMLOCK:{
            qDebug()<<"VK_NUMLOCK"<<VK_NUMLOCK;
            m_bFunLock = true;
            m_pKeyboardVirtualKey->SetVirtualKey(VK_NUMLOCK);
            m_pKeyboardVirtualKey->start();
            break;
        }
        case VK_SCROLL:{
            m_bFunLock = true;
            m_pKeyboardVirtualKey->SetVirtualKey(VK_SCROLL);
            m_pKeyboardVirtualKey->start();
            break;
        }
        case VK_CAPITAL:{
            m_bFunLock = true;
            m_pKeyboardVirtualKey->SetVirtualKey(VK_CAPITAL);
            m_pKeyboardVirtualKey->start();
            break;
        }
        default:
            m_bFunLock = false;
            break;
        }
        b_Pass = true;
    }
    else{
        m_bFunLock = false;
        b_Pass = false;
    }
}

void FilterVkCode::Filter_Enter(int &n_VkCode)
{
    if( n_VkCode == VK_NUMPAD1 ||
        n_VkCode == VK_NUMPAD2 ||
        n_VkCode == VK_NUMPAD3){

        m_bIS_OtherVK_RETURN = true;

    }
    else if(n_VkCode == VK_NUMPAD4 ||
            n_VkCode == VK_NUMPAD5 ||
            n_VkCode == VK_NUMPAD6){
        m_bIS_OtherVK_RETURN = false;
    }

    if(n_VkCode == VK_RETURN){
        if(m_bIS_OtherVK_RETURN)
            n_VkCode = 0xff;
        else
            n_VkCode = VK_RETURN;
    }
}

void FilterVkCode::Filter_Insert(int &n_VkCode)
{
    if( n_VkCode == VK_HOME ||
        n_VkCode == VK_END ||
        n_VkCode == VK_PRIOR ||
        n_VkCode == VK_BACK){

        m_bIS_OtherVK_Fn = true;

    }
    else if(n_VkCode == VK_APPS ||
            n_VkCode == VK_RCONTROL ||
            n_VkCode == VK_RMENU ||
            n_VkCode ==  VK_OEM_2){
        m_bIS_OtherVK_Fn = false;
    }

    if(n_VkCode == VK_INSERT){
        if(m_bIS_OtherVK_Fn)
            n_VkCode = VK_INSERT;
        else
            n_VkCode = 0xe8;
    }
}


void FilterVkCode::slot_HookMsg(int n_VkCode)
{
    Filter_Insert(n_VkCode);
    Filter_Enter(n_VkCode);

    bool b_Pass = false;
    KeyFilter(n_VkCode, b_Pass);

    if(!b_Pass)
        return;

    emit sig_FilterHookMsg(n_VkCode);
}

void FilterVkCode::slot_KeyDown(int n_VkCode)
{
    Filter_Enter(n_VkCode);
    Filter_Insert(n_VkCode);

    qDebug()<<"SendKeyDown "<<n_VkCode;
    emit sig_FilterKeyDown(n_VkCode);
}

void FilterVkCode::slot_KeyUp(int n_VkCode)
{
    Filter_Enter(n_VkCode);
    Filter_Insert(n_VkCode);

    qDebug()<<"SendKeyUp "<<n_VkCode;
    emit sig_FilterKeyUp(n_VkCode);
}
