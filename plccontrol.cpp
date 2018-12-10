#include "plccontrol.h"
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QSettings>

PLCControl*  PLCControl::m_pInstance = NULL;

PLCControl::PLCControl(QObject *parent) : QObject(parent)
{
    m_pOpcClient = OPCClient::GetInstance();
    m_pTagMap = TagMap::GetInstance();

    m_pQThread = NULL;
//    m_pQThread = new QThread;
//    this->moveToThread(m_pQThread);
//    m_pQThread->start();
}

PLCControl::~PLCControl()
{
    if(m_pQThread != NULL){
        m_pQThread->quit();
        m_pQThread->wait();
        delete m_pQThread;
        m_pQThread = NULL;
    }
}

QString PLCControl::makePath(QString Path)
{
    QDir m_Savedir ;
    QString strPath = QDir::currentPath() + Path;

    if(!m_Savedir.exists(strPath))
        m_Savedir.mkdir(strPath);

    return  strPath;
}

bool PLCControl::initOpc()
{
    qDebug()<<"initOpc...";
    if(!m_pOpcClient->isConnected()){
        QString serverName = "KEPware.KEPServerEx.V4";

        bool result = m_pOpcClient->Connect(serverName);
        if(result == S_OK){
            qDebug()<<"connect opc server ok";
            emit sig_Msg(QStringLiteral("OPC服务器打开成功"));
        }
        else{
            qDebug()<<"connect opc sercer error";
            emit sig_Msg(QStringLiteral("OPC服务器打开失败"));
        }
    }

    bool ret = importGroup();
    if(!ret){
        qDebug()<<QStringLiteral("导入组失败");
        emit sig_Msg(QStringLiteral("OPC服务器导入组失败"));
        return false;
    }

    ret = importTag();
    if(!ret){
        qDebug()<<QStringLiteral("导入标签失败");
        emit sig_Msg(QStringLiteral("OPC服务器导入标签失败"));
        return false;
    }

    qDebug()<<QStringLiteral("导入完成");
    return true;
}

bool PLCControl::importGroup()
{
    QString dir = makePath(QString("/Config")) + "/" + FILEGROUP;
    QFile fileGroup(dir);
    if(!fileGroup.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<<"fileGroup open error "<<dir;
        return false;
    }

    QTextStream in_group(&fileGroup);
    QString groupName;
    while(!in_group.atEnd()){
        groupName.clear();
        groupName = in_group.readLine();
        if(m_pOpcClient->AddGroup(groupName, UPDATERATE, ASYNCVALUE)==S_OK){
            connect(m_pOpcClient->m_COPCTransaction, SIGNAL(sg_OnDataChange(OPCHANDLE,DWORD,OPCHANDLE*,VARIANT*,WORD*,FILETIME*,HRESULT*)),
                    this, SLOT(slot_PLCDataChange(OPCHANDLE,DWORD,OPCHANDLE*,VARIANT*,WORD*,FILETIME*,HRESULT*)));
            continue;
        }
        else{
            qDebug()<<"AddGroup error";
            return false;
        }
    }
    return true;
}

bool PLCControl::importTag()
{
    QString dir = makePath(QString("/Config")) + "/" + FILETAG;
    QFile fileTag(dir);
    if(!fileTag.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<<"fileTag open error"<<dir;
        return false;
    }
    QTextStream in_tag(&fileTag);
    QString tag;
    QStringList itemNames;
    while(!in_tag.atEnd()){
        tag = in_tag.readLine();
        itemNames << tag;
    }
    m_pTagMap->initMap(itemNames);
    QStringList r_iNames = m_pOpcClient->AddItems(itemNames);
    if(r_iNames.count() > 0){
        qDebug()<<"add item listen ok";
    }
    else{
        qDebug()<<"add item listen error";
        return false;
    }
    return true;
}

bool PLCControl::setTagValue(QString tagName, QString value)
{
    return m_pOpcClient->WriteValueS(tagName, m_pTagMap->getTypeFromTagName(tagName), value);
}

QString PLCControl::getTagValue(QString q_Name)
{
    QString q_value = m_pTagMap->getValueFromTagName(q_Name);
    return q_value;
}

void PLCControl::slot_PLCDataChange(OPCHANDLE hGroup, DWORD dwCount, OPCHANDLE *phClientItems,
                                    VARIANT *pvValues, WORD *pwQualities, FILETIME *pftTimeStamps, HRESULT *pErrors)
{
    QStringList tagNamesList;
    for(DWORD i = 0; i < dwCount; i++){
        m_pTagMap->setValueAndTypeFromID(phClientItems[i],m_pOpcClient->VariantString(pvValues[i]), (VARENUM)pvValues[i].vt);
        tagNamesList.append(m_pTagMap->getTagNameFromID(phClientItems[i]));
    }

    for(int i = 0; i < tagNamesList.length(); i++){
        QString tagName = tagNamesList.value(i);

        //条件处理此处写入
        emit sig_PLCValueChange(tagName, m_pOpcClient->VariantString(pvValues[i]));
    }
}

void PLCControl::slot_SetPLCData(QString q_tagName, QString q_value)
{
    bool ret = setTagValue(q_tagName, q_value);
    if(!ret){
        qDebug()<<"the data change error";
    }
}
