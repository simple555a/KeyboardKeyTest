#ifndef FINDUSBRECIVER_H
#define FINDUSBRECIVER_H
#include <QObject>
#include <windows.h>
#include <devguid.h>
#include <SetupAPI.h>

#include <setupapi.h>
#pragma comment( lib, "setupapi.lib" )
extern "C" {
#include <hidsdi.h>
}
#pragma comment( lib, "hid.lib" )
#include <Shlwapi.h>
#include <QThread>

class FindUsbReciver : public QObject
{
    Q_OBJECT
public:
    FindUsbReciver(uint vid = 0, uint pid = 0, QObject*parent = 0);
    ~FindUsbReciver();

    //设置PIDVID
    void SetVIDPID(UINT V,UINT P)
    {
        m_unVendorID = V;
        m_unProductID = P;
    }
    //获取PIDVID
    void GetVIDPID(UINT &V,UINT &P)
    {
        V = m_unVendorID;
        P = m_unProductID;
    }

private:
    //查找HID设备
    bool FindHIDDevice();

protected:
    void timerEvent(QTimerEvent *event);
signals:
    //没有找到设备信号
    void sigNotFound();
    //找到设备信号
    void sigFound();

private:
    uint m_unVendorID = 0;
    uint m_unProductID = 0;
    QThread *m_pThread;
    bool m_flag_notfound  = false;  //没有找到标示位
    bool m_flag_found = false;      //找到标志位

};

#endif // FINDUSBRECIVER_H
