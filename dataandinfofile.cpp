#include "dataandinfofile.h"
#include <QDir>
#include <QTextCodec>
#include <QApplication>
#include <QTextStream>
#include <QTime>
#include <QThread>
#include <QDebug>
#include <QXmlStreamReader>

const QString ConfigPath = "/Config/Config.ini";
const QString PIDName = "/pid";
const QString VIDName = "/vid";
const QString UsbIDName = "/usbid";
const QString OccupyName = "/occupy";

const double RectMultiple = 1.3;

const int CompensationHeight  = -120;


DataAndInfoFile*  DataAndInfoFile::m_pDataAndInfoFile = NULL;

DataAndInfoFile::DataAndInfoFile()
{
    m_pQSettings = NULL;
    m_pQFie = NULL;
    InitConfigFile();
//    LogFile();
}

DataAndInfoFile::~DataAndInfoFile()
{
    CloseConfigFile();
}

void DataAndInfoFile::InitConfigFile()
{
    QString filePath = QDir::currentPath() + ConfigPath;
    qDebug()<<"filePath"<<filePath;
    m_pQSettings = new QSettings(filePath, QSettings::IniFormat);
    m_pQSettings->setIniCodec(QTextCodec::codecForName("UTF-8"));

}

void DataAndInfoFile::CloseConfigFile()
{
    if(m_pQFie != NULL){
        m_pQFie->close();
        delete m_pQFie;
        m_pQFie = NULL;
    }

    if(m_pQSettings != NULL){
        m_pQSettings->deleteLater();
        m_pQSettings = NULL;
    }
}

void DataAndInfoFile::ReadKeyboardConfig
    (const QString &str_Type, QMap<int, STRUCT_KEYBOARDINFO> &map_KeyboardInfo)
{

    QString str_FilePath = QDir::currentPath() + "/KeyboardType/" + str_Type + ".xml";

    if(str_FilePath.isEmpty())
        return;

    QFile FileConfig(str_FilePath);
    if(!FileConfig.open(QFile::ReadOnly | QFile::Text)){
        qDebug()<<"open fail"<<str_FilePath;
        return;
    }

    int n_VkCodeData = 0;
    STRUCT_KEYBOARDINFO struct_KBInfoData;
    QXmlStreamReader XmlReader(&FileConfig);

    while(!XmlReader.atEnd() && !XmlReader.hasError()){
        QXmlStreamReader::TokenType token = XmlReader.readNext();
        if(token == QXmlStreamReader::StartDocument)
            //文件开始跳过
            continue;
        if(token == QXmlStreamReader::StartElement){
            if(XmlReader.name() == "device"){
                QXmlStreamAttributes attributes = XmlReader.attributes();
                if(attributes.hasAttribute("caption")){
                    struct_KBInfoData.str_Caption = attributes.value("caption").toString();
                }
            }
            else if(XmlReader.name() == "key"){
                QXmlStreamAttributes attributes = XmlReader.attributes();
                if(attributes.hasAttribute("vkcode")){
                    struct_KBInfoData.n_Vkcode = attributes.value("vkcode").toInt(NULL, 16);
                    n_VkCodeData = attributes.value("vkcode").toInt(NULL, 16);
                }

                if(attributes.hasAttribute("CRect")){

                    QString str_Rect = attributes.value("CRect").toString();
                    QStringList list_Rect = str_Rect.split(',');

                    str_Rect = list_Rect.at(0);
                    struct_KBInfoData.struct_Rect.n_AX = str_Rect.toInt() * RectMultiple;

                    str_Rect = list_Rect.at(1);
                    struct_KBInfoData.struct_Rect.n_AY = str_Rect.toInt() * RectMultiple + CompensationHeight;

                    str_Rect = list_Rect.at(2);
                    struct_KBInfoData.struct_Rect.n_BX = str_Rect.toInt() * RectMultiple;

                    str_Rect = list_Rect.at(3);
                    struct_KBInfoData.struct_Rect.n_BY = str_Rect.toInt() * RectMultiple + CompensationHeight;
                }

                if(attributes.hasAttribute("name")){
                    struct_KBInfoData.str_Name = attributes.value("name").toString();
                }

                if(attributes.hasAttribute("kstatus")){
                    struct_KBInfoData.str_KState = attributes.value("kstatus").toString();
                }
            }
        }

        if(n_VkCodeData != 0)
            map_KeyboardInfo.insert(n_VkCodeData, struct_KBInfoData);
    }
}

void DataAndInfoFile::ReadRecordKeyTemplate(const QString &str_Type, QList<int> &list_Record)
{
    QString str_FilePath = QDir::currentPath() + "/KeyboardType/" + str_Type + "_Record.txt";

    if(str_FilePath.isEmpty())
        return;

    QFile FileConfig(str_FilePath);
    if(!FileConfig.open(QFile::ReadOnly | QFile::Text)){
        qDebug()<<"open fail"<<str_FilePath;
        return;
    }

    QString str_data = "";

    while(!FileConfig.atEnd()){
        str_data = FileConfig.readLine().simplified();
        list_Record.append(str_data.toInt(NULL,16));
    }
}

void DataAndInfoFile::GetPidAndVid(const QString &str_Type, uint &un_Pid, uint &un_Vid)
{
    QString str_PIDName, str_VIDName;
    QString str_pid, str_vid;
    str_PIDName = str_Type + PIDName;
    str_VIDName = str_Type + VIDName;

    str_pid = m_pQSettings->value(str_PIDName).toString();
    str_vid = m_pQSettings->value(str_VIDName).toString();

    un_Pid = str_pid.toUInt(NULL, 16);
    un_Vid = str_vid.toUInt(NULL, 16);
}

void DataAndInfoFile::SavePidAndVid(const QString &str_Type, const uint &un_Pid, const uint &un_Vid)
{
    QString str_PIDName, str_VIDName;
    str_PIDName = str_Type + PIDName;
    str_VIDName = str_Type + VIDName;

    m_pQSettings->setValue(str_PIDName, QString::number(un_Pid,16).toUpper());
    m_pQSettings->setValue(str_VIDName, QString::number(un_Vid,16).toUpper());
}

void DataAndInfoFile::GetUSBID(const QString &str_Type, QString &str_UsbID)
{
    QString str_UsbName = str_Type + UsbIDName;
    str_UsbID = m_pQSettings->value(str_UsbName).toString();
}

void DataAndInfoFile::SaveUSBID(const QString &str_Type, const QString &str_UsbID)
{
    QString str_UsbName = str_Type + UsbIDName;
    m_pQSettings->setValue(str_UsbName, str_UsbID);
}

void DataAndInfoFile::GetOccupy(const QString &str_Type, bool &b_Occupy)
{
    QString str_OccupyName = str_Type + OccupyName;
    b_Occupy = m_pQSettings->value(str_OccupyName).toBool();
}

void DataAndInfoFile::SaveOccupy(const QString &str_Type, const bool &b_Occupy)
{
    QString str_OccupyName = str_Type + OccupyName;
    m_pQSettings->setValue(str_OccupyName, b_Occupy);
}

void DataAndInfoFile::LogFile()
{
//    QDir Savedir ;
//    QString strPath = QDir::currentPath() + "/Log/";

//    if(!Savedir.exists(strPath))
//        Savedir.mkpath(strPath);

//    QDateTime dateNow = dateNow.currentDateTime();
//    strPath = strPath + QString("%1-%2-%3").
//            arg(dateNow.date().year()).arg(dateNow.date().month()).
//            arg(dateNow.date().day()) + "_LOG.txt";

//    m_LastDay = dateNow;

//    m_pQFie = new QFile(strPath);
//    m_pQFie->open(QIODevice::ReadWrite|QIODevice::Append/*|QIODevice::Text*/);
}

QString DataAndInfoFile::AddLog(const QString &strInfo)
{
//    QDateTime dateNow = dateNow.currentDateTime();

//    if(m_LastDay.date().day() != dateNow.date().day() ||
//            m_LastDay.date().month() != m_LastDay.date().month()){
//        if(m_pQFie != NULL){
//            m_pQFie->close();
//            delete m_pQFie;
//            m_pQFie = NULL;
//        }
//        LogFile();
//    }

    QDir Savedir ;
    QString strPath = QDir::currentPath() + "/Log/";

    if(!Savedir.exists(strPath))
        Savedir.mkpath(strPath);

    QDateTime dateNow = dateNow.currentDateTime();
    strPath = strPath + QString("%1-%2-%3").
            arg(dateNow.date().year()).arg(dateNow.date().month()).
            arg(dateNow.date().day()) + "_LOG.txt";

    QFile o_QFile(strPath);

    o_QFile.open(QIODevice::ReadWrite|QIODevice::Append|QIODevice::Text);


    QString strTimeInfo = QString("%1:%2:%3  %4\r\n").
            arg(dateNow.time().hour()).arg(dateNow.time().minute()).
            arg(dateNow.time().second()).arg(strInfo);
    o_QFile.write(strTimeInfo.toStdString().c_str());

    o_QFile.close();

    return strTimeInfo;
}
