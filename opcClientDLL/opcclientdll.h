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

    //�ڴ�������֮ǰ��һ��Ҫ�ȵ��ñ���������OPC���г�ʼ��������ͨѶ�����쳣
    bool init(QString plcName,QString &strInfo, bool haveRemark = false);
    //��ȡ��ǩ��Ӧ����
    QString getValueFromTagName(const QString &tagName);
    //д��ǩ
    bool writeOpc(QString tagName, QString tagValue, VARENUM tagType = VT_EMPTY);
    //�Ͽ�����
    void disConnect();

    QStringList itemNames; //���еı�ǩ����
    QStringList remarkList; //��ǩ��Ӧ������
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
    bool importTag(const QString &plcName,QString &strInfo); //�����ǩ
    bool importGroup(const QString &plcName,QString &strInfo); //������

    //��ȡxml
    bool readXmls(QMap<QString,QMap<QString,QString>> &MapOut,QString &strInfo,QString plcName);
    QString makePath(QString Path);                                                                             //����Ŀ¼
};

#endif // OPCCLIENTDLL_H

