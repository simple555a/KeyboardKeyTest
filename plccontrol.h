#ifndef PLCCONTROL_H
#define PLCCONTROL_H

#include <QObject>
#include "opcClientDLL/opclib/OPCClient.h"
#include "opcClientDLL/tagmap/tagmap.h"
#include <QCoreApplication>
#include <stdint.h>
#include <QString>
#include <QThread>

class PLCControl : public QObject
{
    Q_OBJECT
public:
    explicit PLCControl(QObject *parent = 0);
    ~PLCControl();

    static PLCControl *m_pInstance;
    QString makePath(QString Path);

public:
    static PLCControl * GetInstance()
    {
        if(m_pInstance == NULL)
        {
            m_pInstance = new PLCControl();
        }
        return m_pInstance;
    }

    bool initOpc();
    bool importGroup();
    bool importTag();

    //设置标签值
    bool setTagValue(QString tagName, QString value);
    //获取标签值
    QString getTagValue(QString q_value);
signals:
    //PLC的值更改，信号
    void sig_PLCValueChange(QString tagName, QString value);
    //底层消息
    void sig_Msg(QString);
private slots:
    //PLC数据更改
    void slot_PLCDataChange(OPCHANDLE hGroup,  DWORD dwCount, OPCHANDLE *phClientItems,
                            VARIANT *pvValues, WORD *pwQualities, FILETIME *pftTimeStamps, HRESULT *pErrors);
    //更改PLC的值
    void slot_SetPLCData(QString q_tagName, QString q_value);

public:
    OPCClient *m_pOpcClient;
    TagMap *m_pTagMap;
    QThread *m_pQThread;

};
#endif // PLCCONTROL_H
