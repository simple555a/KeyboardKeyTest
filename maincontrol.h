#ifndef MAINCONTROL_H
#define MAINCONTROL_H
#include "findusbreciver.h"
#include "matchcode.h"
#include "countresult.h"
#include "dataandinfofile.h"
#include "plccontrol.h"

class MainControl : public QObject
{
    Q_OBJECT
public:
    MainControl(QObject* parent = NULL);
    ~MainControl();

    //准备测试
    void PrepareCheck(const WIREDORWIRLESS &m_Type, const QString &str_Type);
    //获取按键结果
    bool GetKeyResult(QMap<int, QList<KEYBOARDRESULTSTATE>> &map_KeyResultShow, KEYTESTFEASIBILITY &Feasibility);

private:
    //初始化部分
    bool InitAllObject();
    //初始配置
    bool InitALLConfig();
    //开始新的测试
    void StartNewCheck();
    //设置键盘的状态
    void SetKeyboardType(const WIREDORWIRLESS &m_Type, const QString &str_Type);
    //获取键盘的状态
    void GetKeyboardType(WIREDORWIRLESS &m_Type, QString &str_Type);
    //设置键盘PIDVID
    void SetKeyboardPIDAndVid(const uint &un_Pid, const uint &un_Vid);
    //过去键盘PIDVID
    void GetKeyboardPIDAndVid(uint &un_Pid, uint &un_Vid);
    //设置键盘USBID
    void SetKeyboardUSBID(QString str_UsbID);
    //设置键盘占用开关
    void SetKeyboardOccupyState(const bool &b_Switch);
    //获取键盘占用开关
    bool GetKeyboardOccupyState();
    //设置PLC 值
    void SetPLCTagValue(const QString &str_TagName, const PLCVALUERESULT &str_Value);
    //发送工作状态
    void SendWorkingState(const WORKINGSTATE &State);

    void SaveLog(QString str_Info);

public:
    void LockKeyboard(bool b_Lock);

signals:
    //消息
    void sig_Msg(QString);
    //重置
    void sig_Reset();
    //工作状态
    void sig_WorkingState(WORKINGSTATE);

private slots:
    void slot_UsbFound();
    void slot_UsbNotFound();
    //对码状态
    void slot_MatchState(bool b_State);
    //PLC的值改变
    void slot_PLCValueChange(QString str_TagName, QString str_Value);
    //底层消息
    void slot_Msg(QString str_Msg);

private:
    FindUsbReciver *m_pFindUsbReciver;
    MatchCode *m_pMatchCode;
    CountResult *m_pCountResult;
    DataAndInfoFile *m_pDataAndInfoFile;
    PLCControl *m_pPLCControl;

    WIREDORWIRLESS m_KeyboardType;      //键盘有线或无线类型
    QString m_strType;                  //键盘型号
    bool m_bUsbReciver;                 //usb接收器状态

    bool m_bCheckAgain;                 //next测试开关
    bool m_bCheckResult;                //检测结果

    QMap<int, QList<KEYBOARDRESULTSTATE>> map_KeyResult;        //按键检测结果
    KEYTESTFEASIBILITY m_Feasibility;                           //检测可能性
};

#endif // MAINCONTROL_H
