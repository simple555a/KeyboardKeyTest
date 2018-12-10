#ifndef TAGMAP_H
#define TAGMAP_H

#include <QObject>
#include <QMap>
#include <wtypes.h>
#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>

class TagMap : public QObject
{
    Q_OBJECT
public:
    TagMap(QObject *parent = 0);
    static TagMap *m_pInstance;
public:
    static TagMap *GetInstance()
    {
         if(m_pInstance == NULL){
            m_pInstance = new TagMap();
         }
         return m_pInstance;
    }
    void initMap(QStringList tagNameList);
    //根据ID 获取标签名
    QString getTagNameFromID(int ID);
    //设置和获取标签对应的 值
    QString getValueFromTagName(QString tagname);
    void setValueInTagName(QString tagname,QString value);
    //设置和获取标签对应的 类型
    VARENUM getTypeFromTagName(QString tagname);
    void setTypeInTagName(QString tagname, enum VARENUM type);
    //保存数据               tagname的id      ,值                  ,类型
    void setValueAndTypeFromID(int id,QString value,enum VARENUM type = VT_R4);
private:
    QStringList tagNameList;  //标签名列表，这个从文件中获取
    QMap<int,QString> ID2tagName; //(id,tagName) //导入到opc的顺序ID，对应的tagname，因为OPC返回的是顺序ID，便于查找
    QMap<QString,QString> tagName2Value; //QMAP(tagName,value)  //保存所有标签对应的值
    QMap<QString,enum VARENUM> tagName2Type; //QMap(tagName,type)  //保存所有标签对应的值的类型，写入的时候需要类型----type为枚举类型，对应返回的VT值

signals:

public slots:
};

#endif // MYMAP_H
