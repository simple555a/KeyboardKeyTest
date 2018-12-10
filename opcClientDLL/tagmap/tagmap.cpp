#include "tagmap.h"
#include <QDebug>

TagMap* TagMap::m_pInstance = NULL;
QReadWriteLock readWritelock;


TagMap::TagMap(QObject *parent) : QObject(parent)
{

}

void TagMap::initMap(QStringList tagNameList1)
{
    tagNameList = tagNameList1;

    for(int i=0;i<tagNameList.length();i++){
        ID2tagName.insert(i,tagNameList.value(i)); //初始化 ID==tagname
        tagName2Type.insert(tagNameList.value(i),VT_EMPTY);//tagname == type 其中type为 enum VARENUM
        tagName2Value.insert(tagNameList.value(i),""); //tagname == value
    }

}

QString TagMap::getTagNameFromID(int ID)
{
    QReadLocker lock(&readWritelock);
    return ID2tagName.value(ID,"null");
}

QString TagMap::getValueFromTagName(QString tagname)
{
    QReadLocker lock(&readWritelock);
    return tagName2Value.value(tagname);
}

void TagMap::setValueInTagName(QString tagname, QString value)
{
    QWriteLocker lock(&readWritelock);
    tagName2Value[tagname] = value;
}

VARENUM TagMap::getTypeFromTagName(QString tagname)
{
    QReadLocker lock(&readWritelock);
    return tagName2Type.value(tagname);
}

void TagMap::setTypeInTagName(QString tagname, VARENUM type)
{
    QWriteLocker lock(&readWritelock);
    tagName2Type[tagname] = type;
}

void TagMap::setValueAndTypeFromID(int id, QString value, VARENUM type)
{
    if(! ID2tagName.contains(id) ){
        qDebug()<<"ID对应的Tagname不存在";
        return;
    }
    QString tagName = getTagNameFromID(id);
    setValueInTagName(tagName,value);
    setTypeInTagName(tagName,type);
}











