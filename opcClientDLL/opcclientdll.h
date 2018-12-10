#ifndef OPCCLIENTDLL_H
#define OPCCLIENTDLL_H

//#include <QtCore/qglobal.h>
//#if defined(OPCCLIENTDLL_LIBRARY)
//#  define OPCCLIENTDLLSHARED_EXPORT Q_DECL_EXPORT
//#else
//#  define OPCCLIENTDLLSHARED_EXPORT Q_DECL_IMPORT
//#endif

#include "opclib/OPCClient.h"
#include "tagmap/tagmap.h"

#include <QDir>
#include <QDomElement>
#include <QTextStream>


class/* OPCCLIENTDLLSHARED_EXPORT*/ OpcClientDll:public QObject
{
  Q_OBJECT
public:
    OpcClientDll();
    ~OpcClientDll();

    //在创建界面之前，一定要先调用本函数，对OPC进行初始化，否则通讯将会异常
    bool init(QString plcName,QString &strInfo, bool haveRemark = false);
    //获取标签对应数据
    QString getValueFromTagName(const QString &tagName);
    //写标签
    bool writeOpc(QString tagName, QString tagValue, VARENUM tagType = VT_EMPTY);
    //断开连接
    void disConnect();

    QStringList itemNames; //所有的标签名字
    QStringList remarkList; //标签对应的描述
signals:
    void sig_dataChange(QStringList tagNameList);
private slots:
    void slot_dataChange(OPCHANDLE hGroup,  DWORD dwCount, OPCHANDLE *phClientItems, VARIANT *pvValues, WORD *pwQualities, FILETIME *pftTimeStamps, HRESULT *pErrors);
private:
    bool haveRemark;
    OPCClient *m_opcClient;
    TagMap *m_tagMap;
    TagMap *tagMap;

private:
    bool importTag(const QString &plcName,QString &strInfo); //导入标签
    bool importGroup(const QString &plcName,QString &strInfo); //导入组

    //读取xml
    bool readXmls(QMap<QString,QMap<QString,QString>> &MapOut,QString &strInfo,QString plcName);
    QString makePath(QString Path);                                                                             //创建目录
};

#endif // OPCCLIENTDLL_H

