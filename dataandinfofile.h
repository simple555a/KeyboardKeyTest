#ifndef DATAANDINFOFILE_H
#define DATAANDINFOFILE_H
#include <QSettings>
#include <QString>
#include <QFile>
#include <QDateTime>
#include "golab_define.h"

class DataAndInfoFile
{
public:
    DataAndInfoFile();
    ~DataAndInfoFile();

    static DataAndInfoFile * GetInstance()
    {
        if(m_pDataAndInfoFile == NULL)
        {
            m_pDataAndInfoFile = new DataAndInfoFile();
        }
        return m_pDataAndInfoFile;
    }

    //初始化
    void InitConfigFile();
    //关闭
    void CloseConfigFile();
    //读取键盘配置参数
    static void ReadKeyboardConfig(const QString &str_Type, QMap<int, STRUCT_KEYBOARDINFO> &map_KeyboardInfo);
    //读取键盘记录数据
    static void ReadRecordKeyTemplate(const QString &str_Type, QList<int> &list_Record);

    //获取PIDVID
    void GetPidAndVid(const QString &str_Type, uint &un_Pid,  uint &un_Vid);
    //保存PIDVID
    void SavePidAndVid(const QString &str_Type, const uint &un_Pid, const uint &un_Vid);
    //获取USBID
    void GetUSBID(const QString &str_Type, QString &str_UsbID);
    //保存USBID
    void SaveUSBID(const QString &str_Type, const QString &str_UsbID);
    //获取键盘占用开关
    void GetOccupy(const QString &str_Type, bool &b_Occupy);
    //保存键盘占用开关
    void SaveOccupy(const QString &str_Type, const bool &b_Occupy);

    //日记本地文件夹
    void LogFile();
    //保存本地日记信息
    QString AddLog(const QString &strInfo);

private:
    static DataAndInfoFile *m_pDataAndInfoFile;     //本类，静态指针
    QSettings *m_pQSettings;                        //配置文件操作指针
    QFile *m_pQFie;                                 //日记文件操作指针
    QDateTime m_LastDay;                            //当天时间
};

#endif // DATAANDINFOFILE_H
