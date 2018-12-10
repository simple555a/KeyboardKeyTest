#include "opcclientdll.h"
#include <QCoreApplication>
#include <QFile>


OpcClientDll::OpcClientDll()
{
    m_opcClient = NULL;
    m_opcClient = new OPCClient();
    m_tagMap = NULL;
    m_tagMap = new TagMap();
    //    xml_ProjectData = new XmlProjectData();
}

OpcClientDll::~OpcClientDll()
{
    if(m_opcClient){
        delete m_opcClient;
        m_opcClient = NULL;
    }
    if(m_tagMap){
        delete m_tagMap;
        m_tagMap = NULL;
    }
}

bool OpcClientDll::init(QString plcName, QString &strInfo, bool haveRemark)
{
    this->haveRemark = haveRemark;
    //????OPC??????
    if(!m_opcClient->isConnected()){
        //serverName???????????????????????????????????????????????????????
        QString serverName = "KEPware.KEPServerEx.V4";
        bool result = m_opcClient->Connect(serverName);
        if(result==S_OK){
            //qDebug()<< "connect opc server ok";
            //strInfo = QStringLiteral("????????????!??????????:") + serverName;
        }else{
            //qDebug()<< "connect opc server error";
            strInfo = QStringLiteral("?????????????!??????????:") + serverName;
            return false;
        }
    }else{
        return false;
    }
    //??????
    //???????????????????§Ü??????
    bool ret = importGroup(plcName,strInfo);
    if( !ret){
        return false;
    }
    /*//???????????????
    connect(m_opcClient->m_COPCTransaction,SIGNAL(sg_OnDataChange(OPCHANDLE,DWORD,OPCHANDLE*,VARIANT*,WORD*,FILETIME*,HRESULT*)),
            this,SLOT(slot_dataChange(OPCHANDLE,DWORD,OPCHANDLE*,VARIANT*,WORD*,FILETIME*,HRESULT*)));
    */
    //qDebug()<<QStringLiteral("?????????");
    //??????
    ret = importTag(plcName,strInfo);
    if( !ret){
        return false;
    }
    //qDebug()<<QStringLiteral("????????");
    strInfo = QStringLiteral("opc????????????");
    return true;
}

QString OpcClientDll::getValueFromTagName(const QString &tagName)
{
    return m_tagMap->getValueFromTagName(tagName);
}
bool OpcClientDll::writeOpc(QString tagName, QString tagValue, VARENUM tagType)
{
    if( VT_EMPTY == tagType){ //??????????????§Õ???????????????????§Ý??
        tagType = m_tagMap->getTypeFromTagName(tagName);
    }
    if(!m_opcClient->isConnected()){
        return false;
    }

    if(m_opcClient->isWriteAble){
        bool ret = m_opcClient->WriteValueS(tagName,tagType,tagValue);
        if(!ret){
            qDebug()<<"write error:"+tagName;
            return false;
        }
    }
    return true;
}

void OpcClientDll::disConnect()
{
    m_opcClient->DisConnect();
}



bool OpcClientDll::importGroup(const QString &plcName, QString &strInfo)
{
    QMap<QString,QMap<QString,QString>> mapOut;
    QString strMessage;

    if(readXmls(mapOut,strMessage,plcName))
    {
        QMap<QString,QString> tagsMap = mapOut.value("Tag");

        QMapIterator <QString,QString>tagListMapIterator(tagsMap);

        while(tagListMapIterator.hasNext())
        {
            tagListMapIterator.next();
            QString groupName = tagListMapIterator.value();

            qDebug()<<"======"<<groupName;
            if(m_opcClient->AddGroup(groupName, UPDATERATE, ASYNCVALUE)==S_OK){     //?????????--???????????--0???????1????
                //qDebug()<< "add group ok";
                //???????????????  ????????????????????
                connect(m_opcClient->m_COPCTransaction,SIGNAL(sg_OnDataChange(OPCHANDLE,DWORD,OPCHANDLE*,VARIANT*,WORD*,FILETIME*,HRESULT*)),
                        this,SLOT(slot_dataChange(OPCHANDLE,DWORD,OPCHANDLE*,VARIANT*,WORD*,FILETIME*,HRESULT*)));

                continue;
            }else{
                //qDebug()<< "AddGroup error";
                strInfo = QStringLiteral("??????????:") + groupName;
                return false;
            }
        }
        return true;
    }
    else
    {
        qDebug()<<strMessage;
        return false;
    }
}

bool OpcClientDll::readXmls(QMap<QString, QMap<QString, QString> > &MapOut, QString &strInfo, QString plcName)
{
    QString strPath = makePath("/Config/") +plcName+ ".xml";

    QFile file(strPath);
    if (!file.open(QIODevice::ReadOnly | QFile::Text))
    {
        strInfo = QStringLiteral("????") + strPath + QStringLiteral("???");
        return false;
    }

    QString errorStr;
    int errorLine;
    int errorColumn;
    QDomDocument doc;
    if (!doc.setContent(&file, false, &errorStr, &errorLine, &errorColumn))
    {
        file.close();
        strInfo = QStringLiteral("???setContent???");
        return false;
    }
    file.close();

    QDomElement root = doc.documentElement();
    if (root.tagName() != "config")
    {
        strInfo = ("??") + strPath + ("?§µ???????config???");
        return false;
    }

    QDomNode node = root.firstChild(); //????????????

    QMap<QString,QString> MapTemp;
    while (!node.isNull())
    {
        QDomElement e = node.toElement();
        if( !e.isNull())
        {
            QDomNodeList list = e.childNodes(); //???????e?????????????§Ò?
            for(int a=0; a<list.count(); a++) //???????§Ò?
            {
                QDomNode node = list.at(a);
                if(node.isElement())
                {
                    MapTemp[node.nodeName()] = node.toElement().text();
                }
            }

            MapOut[e.nodeName()] = MapTemp;
            MapTemp.clear();
        }
        node = node.nextSibling();
    }

    return true;
}

QString OpcClientDll::makePath(QString Path)
{
    QDir m_Savedir ;
    QString strPath = QDir::currentPath() + Path;

    if(!m_Savedir.exists(strPath))
        m_Savedir.mkdir(strPath);

    return  strPath;
}

bool OpcClientDll::importTag(const QString &plcName,QString &strInfo)
{

    QMap<QString,QMap<QString,QString>> mapOut;
    QString strMessage;

    if(readXmls(mapOut,strMessage,plcName))
    {
        QMap<QString,QString> tagsMap = mapOut.value("Tag");

        QMapIterator <QString,QString>tagListMapIterator(tagsMap);

        while(tagListMapIterator.hasNext())
        {
            tagListMapIterator.next();
            QString tagName = tagListMapIterator.value();
            itemNames.append(tagName);
        }
    qDebug()<<itemNames;
    //????§Õ??map??----??????add????§Õ??
    m_tagMap->initMap(itemNames);
    QStringList r_inames= m_opcClient->AddItems(itemNames);
    if(r_inames.count()>0)
    {
        //qDebug()<<"add item listen ok" ;
    }
    else
    {
        //qDebug()<<"add item listen error";
        strInfo = QStringLiteral("?????????");
        return false;
    }
    return true;
}
else
{
qDebug()<<strMessage;
return false;
}
}

//opc ???????
void OpcClientDll::slot_dataChange(OPCHANDLE hGroup,  DWORD dwCount, OPCHANDLE *phClientItems, VARIANT *pvValues, WORD *pwQualities, FILETIME * /*pftTimeStamps*/, HRESULT * /*pErrors*/)
{
    QStringList tagNameList;
    tagNameList.clear();
    int inttr_count = 0;
    QString strTemp;
    for (DWORD i = 0; i < dwCount; i++) {
        inttr_count++;

        //???›Ômap??
        m_tagMap->setValueAndTypeFromID(phClientItems[i],m_opcClient->VariantString(pvValues[i]),(VARENUM)pvValues[i].vt);
        tagNameList.append(m_tagMap->getTagNameFromID(phClientItems[i]));

        //??????????
        strTemp.clear();
        //strTemp = QString::number(inttr_count); //???¦Æ????????????
        strTemp += " ";
        strTemp += QString::number(hGroup); //?????????¨°???
        strTemp += "  ";
        strTemp += QString::number(dwCount);  //??????????????
        strTemp += "  ";
        strTemp += itemNames.at(phClientItems[i]); //
        //strTemp += phClientItems[i]; //??????????????ID???????????????????ID
        strTemp += "  ";
        strTemp += m_opcClient->VariantString(pvValues[i]); //??????????????????????????›ÔMAP??
        strTemp += "  ";
        strTemp += m_opcClient->QualityString(pwQualities[i]); //?????????GOOD ??????????????????????
        strTemp += "  ";

        //?????? FILETIME:?????§Ö?64¦Ë??????????????????????????????UTC??Coordinated Universal Time?????1601??1??1??????100???????¦Ë?????
        /*LPSYSTEMTIME lpst;
        FileTimeToSystemTime(&pftTimeStamps[i], lpst);
        strTemp += QString::number(lpst->wYear); //???????????
        strTemp += QString::number(lpst->wMonth);
        strTemp += QString::number(lpst->wDay);
        strTemp += QString::number(lpst->wHour);
        strTemp += QString::number(lpst->wMinute);
        strTemp += QString::number(lpst->wSecond);*/
        //qDebug()<<"---from gui:"<<strTemp;
    }

    //???¦Ì??????
    //??????? ???¦È?????????§Ò???????????
    emit sig_dataChange(tagNameList);
}


