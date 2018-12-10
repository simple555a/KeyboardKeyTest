#include "usbconrol.h"
#include <QDebug>
#define DEVICE_WIRELESS L"VID_24AE&PID_2010"

HINSTANCE g_hHidDll=0;
OPENHIDDEVICE pOpenHidDevice=0;
GETINPUTREPORT pGetInputReport=0;
FINDHIDDEVICE pFindHidDevice=0;
BOOL g_DebugMode = 0;
const UINT DELAYTIME = 50;

QString UcharToUnicode(UCHAR *puchar, UINT length)
{
    UINT i=0;
    UCHAR *phan;
    WCHAR *pchina;
    QString str;

    phan=new UCHAR[length+2];
    for(i=0;i<length;i++)
        phan[i]=puchar[i];
    phan[length]=0;
    phan[length+1]=0;
    pchina=(WCHAR*)phan;
    str=QString::fromStdWString(pchina);
    delete []phan;
    return str;
}


UsbConrol::UsbConrol(QObject *parent)
    :QObject(parent)
{
    m_hDevice = 0;
    m_bFlag = false;
}

UsbConrol::~UsbConrol()
{
    CloseDevice();
}

bool UsbConrol::InitDevice()
{
    g_hHidDll = LoadLibrary((L"HidParse.dll"));
    if(g_hHidDll){
        pOpenHidDevice = (OPENHIDDEVICE)GetProcAddress(g_hHidDll,"OpenHidDevice");
        pGetInputReport = (GETINPUTREPORT)GetProcAddress(g_hHidDll,"GetInputReport");
        pFindHidDevice = (FINDHIDDEVICE)GetProcAddress(g_hHidDll,"FindHidDevice");
    }
    if(!g_hHidDll || !pOpenHidDevice || !pGetInputReport){
        qDebug("load dll failed.");
        return false;
    }

    WCHAR wc_ID[20];
    for(int i = 0;i < sizeof(wc_ID)/sizeof(WCHAR);i++){
        wc_ID[i] = (WCHAR)0x0000;
    }

    if(m_strID.toWCharArray(wc_ID) == 0){
        qDebug("m_strID is error.");
        return false;
    }

//    qDebug()<<"m_strID"<<m_strID;
//    qDebug()<<"const_cast<WCHAR*> (DEVICE_WIRELESS)"<<QString::fromWCharArray(const_cast<WCHAR*> (DEVICE_WIRELESS));
//    qDebug()<<"wc_ID"<<QString::fromWCharArray(wc_ID);

//    const wchar_t * wc_pData = reinterpret_cast<const wchar_t *>(m_strID.utf16());
//    wchar_t wc_Data[20] = {};
//    memcpy(wc_Data, wc_pData, wcslen(wc_pData) + 1);

    qDebug()<<"wcslen1"<<wcslen(const_cast<WCHAR*>(DEVICE_WIRELESS));
    qDebug()<<"wcslen2"<<wcslen(wc_ID);

    m_hDevice = pOpenHidDevice(0xff00, 0x0e, /*const_cast<WCHAR*>(DEVICE_WIRELESS)*/wc_ID, NULL, 0);

    m_bFlag  = (m_hDevice)?true:false;
    qDebug()<<"m_bFlag"<<m_bFlag;

    qDebug()<<"m_hDevice"<<m_hDevice;

    return (m_hDevice)?true:false;
}

bool UsbConrol::CloseDevice()
{
    m_bFlag  = false;
    if (m_hDevice)
        CloseHandle(m_hDevice);
    if (g_hHidDll)
        FreeLibrary(g_hHidDll);
    return true;
}

bool UsbConrol::GetInitState()
{
    return m_bFlag;
}

bool UsbConrol::SetIDString(QString &str_ID)
{
    m_strID = str_ID;
    return true;
}

QString UsbConrol::GetIDString()
{
    return m_strID;
}

bool UsbConrol::SendCommand(BYTE *uc_Buffer, UINT un_Size)
{
    if (m_hDevice == NULL || !uc_Buffer || un_Size>(0x20 - 1))
        return false;

    DWORD bytesWritten = 0;
    BYTE Buffer[0x21];
    memset(Buffer, 0, sizeof(Buffer));
    Buffer[0] = (BYTE)0xba;
    memcpy(&Buffer[1], uc_Buffer, un_Size);

    if (!WriteFile(m_hDevice, Buffer, 0x20, &bytesWritten, NULL)){
        qDebug("WriteFile Failed, Error Code %d\n", GetLastError());
        return false;
    }

    return true;
}

bool UsbConrol::GetCommand(BYTE *uc_Buffer, UINT un_Size)
{
    if (m_hDevice == NULL || !uc_Buffer || un_Size>(0x20 - 1))
        return false;

//    DWORD bytesRead = 0;
    BYTE Buffer[0x20];
    memset(Buffer, 0, sizeof(Buffer));
    Buffer[0] = (BYTE)0xba;
    memcpy(&Buffer[1], uc_Buffer, un_Size);

    if (!pGetInputReport(m_hDevice, Buffer, 0x20)){
        qDebug("GetInputReport Failed, Error Code %d\n", GetLastError());
        return false;
    }

    memcpy(uc_Buffer, Buffer, un_Size);
    return true;
}
