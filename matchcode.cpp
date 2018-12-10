#include "matchcode.h"
#include <QDebug>
#include <QTime>
#include <QCoreApplication>
#include <QEventLoop>

MatchCode::MatchCode(QObject *parent)
    : QObject(parent)
{
    m_pUsbConrol = NULL;
    m_pQTimer = NULL;

    m_nMatchWaitTime = 0;

    m_pUsbConrol = new UsbConrol(this);
    m_pQTimer = new QTimer;
    connect(m_pQTimer, &QTimer::timeout, this, &MatchCode::slot_TimerOut);


    m_pQThread = new QThread;
    this->moveToThread(m_pQThread);
    m_pQThread->start();
}

MatchCode::~MatchCode()
{
    if(!m_pQTimer){
        if(m_pQTimer->isActive()){
            m_pQTimer->stop();
        }

        delete m_pQTimer;
        m_pQTimer = NULL;
    }

    if(m_pQThread != NULL){
        m_pQThread->quit();
        m_pQThread->wait();
        delete m_pQThread;
        m_pQThread = NULL;
    }
}

bool MatchCode::InitUSBDevice()
{
    return m_pUsbConrol->InitDevice();
}

bool MatchCode::CloseUSBDevice()
{
    return m_pUsbConrol->CloseDevice();
}

bool MatchCode::SetUSBID(QString &str_ID)
{
    return m_pUsbConrol->SetIDString(str_ID);
}

bool MatchCode::SendCommandDongle(BYTE *uc_Buffer, UINT un_Size)
{
    WorkSleep(50);
    if(m_pUsbConrol->SendCommand(uc_Buffer, un_Size)){
        BYTE Cmd[2] = {0x00, 0x00};
        for(int i = 0; i<5 && 0x01 != Cmd[0]; i++){
            WorkSleep(50);
            m_pUsbConrol->GetCommand(Cmd, 2);
            qDebug()<<"GetCommand]"<<Cmd[0];
            if(Cmd[0] == 0x01)
                return true;
        }
        return false;
    }
    else{
        return false;
    }
}

bool MatchCode::GetKeyboardKeyboardMode()
{
    qDebug()<<"GetKeyboardKeyboardMode";
    BYTE Cmd[4]={0xa7};
    if(SendCommandDongle(Cmd, 1)){
        BYTE Cmd[2] = {0x00, 0x00};
        for(int i = 0; i<5 && 0x01 != Cmd[0]; i++){
            WorkSleep(50);
            qDebug()<<"KeyboardMode Cmd[1]"<<Cmd[1];
            m_pUsbConrol->GetCommand(Cmd, 2);
            if(Cmd[1] != 0xa1){
                qDebug()<<"in KeyboardMode";
                return true;
            }
        }
        return false;
    }
    return false;
}

bool MatchCode::EnterMatchModel()
{
    qDebug()<<"EnterMatchModel";
    BYTE Cmd[3]={0xa0, 0x81, 0x02};
    return SendCommandDongle(Cmd, 3);
}

bool MatchCode::ClearKeyboardMatch()
{
    qDebug()<<"ClearKeyboardMatch";
    BYTE Cmd[2]={0xa1, 0x83};
    return SendCommandDongle(Cmd, 2);
}

bool MatchCode::ExitMatchModel()
{
    qDebug()<<"ExitMatchModel";
    BYTE Cmd[2]={0xa1, 0x84};
    return SendCommandDongle(Cmd, 2);
}

bool MatchCode::GetMatchState()
{
    qDebug()<<"GetMatchState";
    BYTE Cmd[2]={0xa1, 0x01};
    if(m_pUsbConrol->SendCommand(Cmd, 2)){
        BYTE Cmd[4] = {0x00, 0x00, 0x00, 0x00};
        for(int i = 0; i<5 && 0x01 != Cmd[0]; i++){
            WorkSleep(50);
            m_pUsbConrol->GetCommand(Cmd, 4);
            if(Cmd[1] != 0 && 0x01 == Cmd[0]){
                return true;
            }
        }
        return false;
    }
    else{
        return false;
    }
}

bool MatchCode::StartMatch()
{
    WorkSleep(50);
    ExitMatchModel();
    WorkSleep(50);
    EnterMatchModel();

    m_pQTimer->start(1000);
    return true;
}

bool MatchCode::GetIsInit()
{
    return m_pUsbConrol->GetInitState();
}

void MatchCode::WorkSleep(int n_Msec)
{
    QTime o_DieTime = QTime::currentTime().addMSecs(n_Msec);
    while(QTime::currentTime() < o_DieTime ){
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        QThread::msleep(10);
    }
}

void MatchCode::slot_TimerOut()
{
    if(m_nMatchWaitTime == 30){
        m_pQTimer->stop();
        m_nMatchWaitTime = 0;
        emit sig_Match(false);
        ExitMatchModel();
        return;
        //对码超时
    }

    if(GetMatchState()){
        m_pQTimer->stop();
        m_nMatchWaitTime = 0;
        emit sig_Match(true);
        ExitMatchModel();
        return;
    }
    else{
        m_nMatchWaitTime ++;
    }
}
