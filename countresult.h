#ifndef COUNTRESULT_H
#define COUNTRESULT_H

#include <QObject>
#include <QString>
#include <QMap>
#include "golab_define.h"
#include "filtervkcode.h"

class CountResult : public QObject
{
    Q_OBJECT
public:
    CountResult(QObject *parent = NULL);
    //设置键盘信息占用开关
    bool SetKeyboardOccupySwitch(const bool &b_Switch);
    //获取键盘信息占用开关
    bool GetKeyboardOccupySwitch();
    //设置键盘信息信号锁
    bool SetKeyboardInfoLock(const bool &b_Lock);

    //新的检测
    bool StartNewCheck();
    //获取结果，按键次数检测，（暂不用）
    bool GetResult(const QString &str_Type,
                   bool &b_Result);
    //获取结果，按键排序检测
    bool GetResult(const QString &str_Type,
                   QMap<int, QList<KEYBOARDRESULTSTATE>> &map_Result,
                   KEYTESTFEASIBILITY &Feasibility,
                   bool &b_Result);
    //初始化钩子
    void StartHook();
    //停止钩子
    void StopHook();
private:
    //检测数据，获取结果，按键测试检测，（暂不用）
    bool GetCheckData(const QString &str_Type,
                      QMap<int, bool> &map_KeyboardCheck);
    //检测数据，获取结果，按键排序检测
    void GetCheckData(const QString &str_Type,
                         QList<int> &list_Acquire,
                         QMap<int, QList<KEYBOARDRESULTSTATE> > &map_Result,
                         KEYTESTFEASIBILITY &Feasibility);

    //找个按键个数为0
    void FindEqualToZero(int &n_ListIndex,
                         QList<int> &list_Record,
                         QList<int> &list_Acquire,
                         QMap<int,QList<KEYBOARDRESULTSTATE>> &map_Result);
    //找个按键个数为1
    void FindEqualToOne(int &n_ListIndex,
                        QList<int> &list_Record,
                        QList<int> &list_Acquire,
                        QMap<int,QList<KEYBOARDRESULTSTATE>> &map_Result);
    //找个按键个数大于0
    void FindMoreThanOne(int &n_ListIndex,
                         int &n_TNumber,
                         QList<int> &list_Record,
                         QList<int> &list_Acquire,
                         QMap<int, QList<KEYBOARDRESULTSTATE>> &map_Result);
    //插入数据进入map存放
    QList<KEYBOARDRESULTSTATE> InsertDataToList(int n_ListT,
                                          QMap<int, QList<KEYBOARDRESULTSTATE> > &map_Result,
                                          const KEYBOARDRESULTSTATE &k_State);

private slots:
    //接收键盘按键信息
    void slot_UpVkCode(int n_VkCode);

private:
    FilterVkCode *m_pFilterVkCode;      //过滤键盘按键信息
    QMap<int,int> map_KeyboardVkCode;   //键盘按键记录数据
    QList<int> list_AcquireVkCode;      //获取按键实时数据
};

#endif // COUNTRESULT_H
