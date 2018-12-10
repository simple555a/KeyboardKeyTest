#include "findusbreciver.h"
#include <QDebug>


FindUsbReciver::FindUsbReciver(uint vid, uint pid, QObject *parent)
    :QObject(parent)
{
    m_pThread = NULL;

    m_unVendorID = vid;
    m_unProductID = pid;
    startTimer(1000);

    m_pThread = new QThread;
    moveToThread(m_pThread);
    m_pThread->start();
}

FindUsbReciver::~FindUsbReciver()
{
    if(m_pThread->isRunning()){
        m_pThread->quit();
        m_pThread->wait();
        m_pThread->deleteLater();
    }
}

bool FindUsbReciver::FindHIDDevice()
{
    GUID    HidGuid;
    HANDLE	hDevInfo;
    SP_DEVICE_INTERFACE_DATA devInfoData;
    HIDD_ATTRIBUTES Attributes;
    HANDLE	DeviceHandle = INVALID_HANDLE_VALUE;
    bool	Result;
    ULONG	Lenth;
    ULONG	Required;
    bool	LastDevice;//檢測是否到最后一個設備
    int		MemberIndex;

    MemberIndex = 0;
    LastDevice = FALSE;
    //獲取HID設備的GUID，并保存在HidGuid中
    HidD_GetHidGuid(&HidGuid);

    //	listBox1->Items->Add("開始查找設備");
    hDevInfo = SetupDiGetClassDevs(&HidGuid, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
    //對devInfoData結構體cbsize初始化為結構體大小
    devInfoData.cbSize = sizeof(devInfoData);
    //對Attributes結構體的Size初始化為結構體大小
    Attributes.Size = sizeof(Attributes);

    while (LastDevice == FALSE){

        //調用該函數之前必須先設置DeviceInterfaceData.cbSize
        //從設備信息集合中獲取一個設備接口信息的函數
        //MemberIndex初始值必須為0. Then, repeatedly increment MemberIndex and retrieve an interface until this function fails

        Result = SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &HidGuid, MemberIndex, &devInfoData);
        //如果获取信息失败，则说明设备已经查找完毕，退出循环。
        if(Result == false)
            break;

        if (Result){
            // One device add
            MemberIndex++;

            //設備獲取成功，繼續獲取設備的詳細信息，在獲取
            //详细信息时，需要先知道保存详细信息需要多大的缓冲区，这通过
            //第一次调用函数SetupDiGetDeviceInterfaceDetail来获取。这时
            //提供缓冲区和长度都为NULL的参数，并提供一个用来保存需要多大
            //缓冲区的变量RequiredSize。
            /*函數原型
                SetupDiGetDeviceInterfaceDetail
                (IN HDEVINFO  DeviceInfoSet,
                IN PSP_DEVICE_INTERFACE_DATA  DeviceInterfaceData,
                OUT PSP_DEVICE_INTERFACE_DETAIL_DATA  DeviceInterfaceDetailData..OPTIONAL,
                IN DWORD  DeviceInterfaceDetailDataSize,
                OUT PDWORD  RequiredSize..OPTIONAL,
                OUT PSP_DEVINFO_DATA  DeviceInfoData  OPTIONAL);
                */
            //這時設置DeviceInterfaceDetailData=NULL,DeviceInterfaceDetailDataSize=0
            //獲取緩沖區大小保存為Lenth
            Result = SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInfoData, NULL, NULL, &Lenth, NULL);
            //分配一個大小為Lenth的緩沖區，用來保存設備詳細信息
            PSP_DEVICE_INTERFACE_DETAIL_DATA detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(Lenth);
            if (detailData == NULL) {
                //如果内存不足，则直接返回
                qDebug("Out of memory");
                SetupDiDestroyDeviceInfoList(hDevInfo);
                return FALSE;
            }
            //設置detailData的cbSize為結構體的大小（只是結構體大小不包括后面緩沖區）
            detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
            //再次調用SetupDiGetDeviceInterfaceDetail設置所使用的緩沖區及緩沖區大小
            Result = SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInfoData, detailData, Lenth, &Required, NULL);
            //如果调用成功，则使用不带读写访问的CreateFile函数
            //来获取设备的属性，包括VID、PID、版本号等。
            //对于一些独占设备（例如USB键盘），使用读访问方式是无法打开的，lll
            //而使用不带读写访问的格式才可以打开这些设备，从而获取设备的属性。

            QString strDevicePath = QString::fromStdWString( detailData->DevicePath);
            free(detailData);
            DeviceHandle = CreateFile(strDevicePath.toStdWString().c_str(),
                                      NULL,//GENERIC_WRITE,//GENERIC_READ,////|||
                                      FILE_SHARE_READ | FILE_SHARE_WRITE,
                                      NULL,
                                      OPEN_EXISTING,
                                      FILE_FLAG_BACKUP_SEMANTICS,//FILE_ATTRIBUTE_NORMAL,//|FILE_FLAG_OVERLAPPED,//FILE_FLAG_BACKUP_SEMANTICS
                                      NULL);

            if (INVALID_HANDLE_VALUE == DeviceHandle){
                qDebug("INVALID_HANDLE_VALUE");
                continue;
            }

            //獲取設備屬性并保存到Attributes結構體中
            Result = HidD_GetAttributes(DeviceHandle, &Attributes);

            if (FALSE == Result){
                CloseHandle(DeviceHandle);
                continue;
            }
            if (Attributes.VendorID == m_unVendorID && Attributes.ProductID == m_unProductID){
                CloseHandle(DeviceHandle);
                return TRUE;
            }
            else{
                continue;
                CloseHandle(DeviceHandle);
            }
        }
        else{
            //SetupDiEnumDeviceInterfaces返回0，代表設備已經查找完畢，退出循環
            CloseHandle(DeviceHandle);
            LastDevice = TRUE;
        }
    }
    //調用SetupDiDestroyDeviceInfoList銷毀設備信息集合
    SetupDiDestroyDeviceInfoList(hDevInfo);
    return false;
}

void FindUsbReciver::timerEvent(QTimerEvent *event)
{
    if(!FindHIDDevice()){
       if(!m_flag_notfound){
           m_flag_notfound = true;
           m_flag_found  = false;
           emit sigNotFound();
       }
    }
    else{
        if(!m_flag_found){
            m_flag_found = true;
            m_flag_notfound  = false;
            emit sigFound();
        }
    }
    QObject::timerEvent(event);
}
