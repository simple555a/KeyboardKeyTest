#ifndef USBCONROL_H
#define USBCONROL_H
#include <QObject>

#include <windows.h>
#include <WinUser.h>
#include <devguid.h>
#include <SetupAPI.h>

#include <setupapi.h>
#pragma comment( lib, "setupapi.lib" )
extern "C" {
#include <hidsdi.h>
}
#pragma comment( lib, "hid.lib" )
#include <Shlwapi.h>

QString UcharToUnicode(UCHAR *puchar, UINT length);


struct USBHANDLE{
    USBHANDLE()
    {}
    USBHANDLE(QString n,HANDLE r,HANDLE w,
               USHORT len)
    {
        name = n;
        hReadHandle = r;
        hWriteHandle = w;
        ReportByteLength = len;

    }
    QString name;
    HANDLE hReadHandle = INVALID_HANDLE_VALUE;
    HANDLE hWriteHandle = INVALID_HANDLE_VALUE;
    UINT ReportID;
    USHORT ReportByteLength;
};


typedef HANDLE (*OPENHIDDEVICE)(USHORT UsagePage,
                                USHORT Usage,
                                WCHAR *idString,
                                WCHAR *szDevPath,     //若该指针非空，则函数将设备路径打印到该缓冲区中
                                DWORD dwLength       //szDevPath缓冲区的长度, 若缓冲区长度不够, 则设备路径将被截断
                                );
typedef BOOLEAN (*GETINPUTREPORT)(HANDLE HidDeviceObject, PVOID ReportBuffer, ULONG ReportBufferLength);
typedef DWORD (*FINDHIDDEVICE)(WCHAR *idString);

extern HINSTANCE g_hHidDll;
extern OPENHIDDEVICE pOpenHidDevice;
extern GETINPUTREPORT pGetInputReport;
extern FINDHIDDEVICE pFindHidDevice;


class UsbConrol : public QObject
{
    Q_OBJECT
public:
    UsbConrol(QObject*parent = 0);
    ~UsbConrol();
    bool InitDevice();
    bool CloseDevice();
    bool GetInitState();

    //设置USBID
    bool SetIDString(QString &str_ID);
    //获取USBID
    QString GetIDString();
    //写入数据
    bool SendCommand(BYTE* uc_Buffer, UINT un_Size);
    //获取数据
    bool GetCommand(BYTE* uc_Buffer, UINT un_Size);

private:
    HANDLE m_hDevice;
    bool m_bFlag;
    QString m_strID;    //L"VID_24AE&PID_4130"

};

#endif // USBCONROL_H
