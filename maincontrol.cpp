 #include "maincontrol.h"

MainControl::MainControl(QObject *parent)
    : QObject(parent)
{ 
    m_strType           = "";
    m_bUsbReciver       = false;
    m_bCheckAgain       = false;
    m_bCheckResult      = false;

    m_pFindUsbReciver   = NULL;
    m_pMatchCode        = NULL;
    m_pCountResult      = NULL;
    m_pDataAndInfoFile  = NULL;
    m_pPLCControl       = NULL;

    InitAllObject();
}

MainControl::~MainControl()
{

}

void MainControl::PrepareCheck(const WIREDORWIRLESS &m_Type, const QString &str_Type)
{
    SetKeyboardType(m_Type, str_Type);
    InitALLConfig();
    SendWorkingState(READY_S);
}

bool MainControl::GetKeyResult(QMap<int, QList<KEYBOARDRESULTSTATE> > &map_KeyResultShow, KEYTESTFEASIBILITY &Feasibility)
{
    if(map_KeyResult.isEmpty())
        return false;

    map_KeyResultShow = map_KeyResult;
    Feasibility = m_Feasibility;
    return true;
}

bool MainControl::InitAllObject()
{
    m_pFindUsbReciver   = new FindUsbReciver;
    m_pMatchCode        = new MatchCode;
    m_pCountResult      = new CountResult;
    m_pDataAndInfoFile  = DataAndInfoFile::GetInstance();
    m_pPLCControl       = PLCControl::GetInstance();

    connect(m_pFindUsbReciver, SIGNAL(sigFound()),
            this, SLOT(slot_UsbFound()));
    connect(m_pFindUsbReciver, SIGNAL(sigNotFound()),
            this, SLOT(slot_UsbNotFound()));

    connect(m_pMatchCode, SIGNAL(sig_Match(bool)),
            this, SLOT(slot_MatchState(bool)));

    connect(m_pPLCControl, SIGNAL(sig_PLCValueChange(QString,QString)),
            this, SLOT(slot_PLCValueChange(QString,QString)));
    connect(m_pPLCControl, SIGNAL(sig_Msg(QString)),
            this, SLOT(slot_Msg(QString)));

    m_pPLCControl->initOpc();

    m_pCountResult->StartHook();

    return true;
}

bool MainControl::InitALLConfig()
{
    uint un_Pid, un_Vid;
    QString str_UsbID;
    bool b_Occupy;

    m_pDataAndInfoFile->GetPidAndVid(m_strType, un_Pid, un_Vid);
    m_pDataAndInfoFile->GetUSBID(m_strType, str_UsbID);
    m_pDataAndInfoFile->GetOccupy(m_strType, b_Occupy);

    SetKeyboardPIDAndVid(un_Pid, un_Vid);
    SetKeyboardUSBID(str_UsbID);
    SetKeyboardOccupyState(b_Occupy);

    return true;
}

void MainControl::StartNewCheck()
{
    m_bCheckAgain = false;
    m_pCountResult->StartNewCheck();

    if(!map_KeyResult.isEmpty()){
        map_KeyResult.clear();
    }

    m_Feasibility = CANTEST;
    emit sig_Reset();
}

void MainControl::SetKeyboardType(const WIREDORWIRLESS &m_Type, const QString &str_Type)
{
    m_KeyboardType = m_Type;
    m_strType = str_Type;
}

void MainControl::GetKeyboardType(WIREDORWIRLESS &m_Type, QString &str_Type)
{
    m_Type = m_KeyboardType;
    str_Type = m_strType;
}

void MainControl::SetKeyboardPIDAndVid(const uint &un_Pid, const uint &un_Vid)
{
    m_pFindUsbReciver->SetVIDPID(un_Vid, un_Pid);
}

void MainControl::GetKeyboardPIDAndVid(uint &un_Pid, uint &un_Vid)
{
    m_pFindUsbReciver->GetVIDPID(un_Vid, un_Pid);
}

void MainControl::SetKeyboardUSBID(QString str_UsbID)
{
    m_pMatchCode->SetUSBID(str_UsbID);
}

void MainControl::SetKeyboardOccupyState(const bool &b_Switch)
{
    m_pCountResult->SetKeyboardOccupySwitch(b_Switch);
}

bool MainControl::GetKeyboardOccupyState()
{
    return m_pCountResult->GetKeyboardOccupySwitch();
}

void MainControl::SetPLCTagValue(const QString &str_TagName, const PLCVALUERESULT &str_Value)
{
    m_pPLCControl->setTagValue(str_TagName, QString::number(str_Value));
}

void MainControl::SendWorkingState(const WORKINGSTATE &State)
{
    emit sig_WorkingState(State);
}

void MainControl::SaveLog(QString str_Info)
{
    m_pDataAndInfoFile->AddLog(str_Info);
}

void MainControl::LockKeyboard(bool b_Lock)
{
    m_pCountResult->SetKeyboardInfoLock(b_Lock);
}

void MainControl::slot_UsbFound()
{
    qDebug()<<"UsbFound";
    SaveLog("UsbFound");
    m_bUsbReciver = true;
    SetPLCTagValue(PLC_FindUSB, Find);
    if(m_KeyboardType == WIRLESS){
        if(!m_pMatchCode->GetIsInit()){
            m_pMatchCode->CloseUSBDevice();
            m_pMatchCode->InitUSBDevice();
        }
    }
}

void MainControl::slot_UsbNotFound()
{
    qDebug()<<"UsbNotFound";
    m_bUsbReciver = false;
    SaveLog("UsbNotFound");
    SetPLCTagValue(PLC_FindUSB, NoFind);
    if(m_KeyboardType == WIRLESS){
        if(m_pMatchCode->GetIsInit()){
            m_pMatchCode->CloseUSBDevice();
        }
    }
}

void MainControl::slot_MatchState(bool b_State)
{
    if(b_State){
        SetPLCTagValue(PLC_Match_OK, OK);
        SendWorkingState(MATCH_OK_S);
        qDebug()<<"MATCH_OK_S";
        SaveLog("MATCH_OK_S");
    }
    else{
    SetPLCTagValue(PLC_Match_NG, NG);
    SendWorkingState(MATCH_NG_S);
    qDebug()<<"MATCH_NG_S";
    SaveLog("MATCH_NG_S");
    }

//    m_pCountResult->SetKeyboardInfoLock(false);
}
void MainControl::slot_PLCValueChange(QString str_TagName, QString str_Value)
{
    int u_Result = str_Value.toInt();

    if(str_TagName == PLC_Ready && u_Result == Ready){
        StartNewCheck();
        SendWorkingState(RUN_S);
        SetPLCTagValue(PLC_Ready, Reset);

        if(!m_bUsbReciver)
            return;

        if(m_KeyboardType == WIRED && m_bUsbReciver){
            SetPLCTagValue(PLC_Match_OK, OK);
        }
        else if(m_KeyboardType == WIRLESS && m_bUsbReciver){
            m_pMatchCode->StartMatch();
//            m_pCountResult->SetKeyboardInfoLock(true);
            qDebug()<<"StartMatch";
        }
        else{
            SetPLCTagValue(PLC_Match_NG, NG);
            SendWorkingState(ERROR_S);
        }
    }
    else if(str_TagName == PLC_StartKey && u_Result == StartKey){
        SendWorkingState(RUN_S);
        SetPLCTagValue(PLC_StartKey, Reset);
//        m_pCountResult->SetKeyboardInfoLock(true);
    }
    else if(str_TagName == PLC_FinishKey && u_Result == FinishKey){
        bool b_Result;
        m_pCountResult->GetResult(m_strType,
                                  map_KeyResult,
                                  m_Feasibility,
                                  b_Result);

        if(b_Result){
           SetPLCTagValue(PLC_Result_OK, OK);
           SendWorkingState(PASS_S);
        }
        else{
           SetPLCTagValue(PLC_Result_NG, NG);
           SendWorkingState(ERROR_S);
        }

        SetPLCTagValue(PLC_FinishKey, Reset);
        m_bCheckAgain = !b_Result;
//        m_pCountResult->SetKeyboardInfoLock(false);
    }
    else{

    }
}

void MainControl::slot_Msg(QString str_Msg)
{
    str_Msg = m_pDataAndInfoFile->AddLog(str_Msg);
    emit sig_Msg(str_Msg);
}
