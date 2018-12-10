#ifndef MATCHCODE_H
#define MATCHCODE_H
#include <QObject>
#include <QTimer>
#include "usbconrol.h"
#include <QThread>

class MatchCode : public QObject
{
    Q_OBJECT
public:
    MatchCode(QObject *parent = 0);
    ~MatchCode();

    bool InitUSBDevice();
    bool CloseUSBDevice();
    bool SetUSBID(QString &str_ID);
    //dongle 写数据接口
    bool SendCommandDongle(BYTE* uc_Buffer, UINT un_Size);
    //获取键盘状态
    bool GetKeyboardKeyboardMode();
    //进入对码模式
    bool EnterMatchModel();
    //获取对码模式
    bool GetMatchState();
    //清除对码状态
    bool ClearKeyboardMatch();
    //退出对码状态
    bool ExitMatchModel();
    //开始对码
    bool StartMatch();
    //获取初始化状态
    bool GetIsInit();

private:
    void WorkSleep(int n_Msec);

signals:
    //发送对码状态
    void sig_Match(bool);
private slots:
    //定期器结构
    void slot_TimerOut();

private:
    UsbConrol *m_pUsbConrol;
    QTimer *m_pQTimer;
    QThread *m_pQThread;

    int m_nMatchWaitTime;   //对码等待时间
};

#endif // MATCHCODE_H
