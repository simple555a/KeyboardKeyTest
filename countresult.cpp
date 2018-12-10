#include "countresult.h"
#include "dataandinfofile.h"
#include <QDebug>

CountResult::CountResult(QObject *parent)
    : QObject(parent)
{
    m_pFilterVkCode = NULL;
    m_pFilterVkCode = FilterVkCode::GetInstance();

    connect(m_pFilterVkCode, SIGNAL(sig_FilterHookMsg(int)),
            this, SLOT(slot_UpVkCode(int)), Qt::QueuedConnection);
}

bool CountResult::SetKeyboardOccupySwitch(const bool &b_Switch)
{
    return m_pFilterVkCode->SetFilterHookOccupy(b_Switch);
}

bool CountResult::GetKeyboardOccupySwitch()
{
    return m_pFilterVkCode->GetFilterHookOccupy();
}

bool CountResult::SetKeyboardInfoLock(const bool &b_Lock)
{
    return m_pFilterVkCode->SetFilterHookInfoLock(b_Lock);
}

bool CountResult::StartNewCheck()
{
    if(!map_KeyboardVkCode.isEmpty())
        map_KeyboardVkCode.clear();

    if(!list_AcquireVkCode.isEmpty())
        list_AcquireVkCode.clear();

    return true;
}

bool CountResult::GetResult(const QString &str_Type,
                            bool &b_Result)
{
    QMap<int, bool> map_KeyboardCheck;
    GetCheckData(str_Type, map_KeyboardCheck);

    b_Result = false;
    if(map_KeyboardCheck.isEmpty())
        return false;

    b_Result = true;
    QMapIterator<int, bool> map_IteratorCheck(map_KeyboardCheck);
    while (map_IteratorCheck.hasNext()){
        map_IteratorCheck.next();

        if(!map_IteratorCheck.value()){
            b_Result = false;
            qDebug()<<"CheckResultvk"<<map_IteratorCheck.key();
            map_IteratorCheck.toBack();
        }
    }
    return true;
}

bool CountResult::GetResult(const QString &str_Type,
                            QMap<int, QList<KEYBOARDRESULTSTATE> > &map_Result,
                            KEYTESTFEASIBILITY &Feasibility,
                            bool &b_Result)
{
     b_Result = false;
    if(list_AcquireVkCode.isEmpty())
        return false;

    GetCheckData(str_Type, list_AcquireVkCode, map_Result, Feasibility);

    if(Feasibility == CANNOTTEST)
        return true;

    b_Result = true;

    //检验每一个按键测试状态，若有一个不是NOERROR_K，则测试失败
    QMapIterator<int, QList<KEYBOARDRESULTSTATE> > map_IteratorCheck(map_Result);
    while (map_IteratorCheck.hasNext()){
        map_IteratorCheck.next();

        if(!map_IteratorCheck.value().contains(NOERROR_K)){
            b_Result = false;
            qDebug()<<"CheckResultvk"<<map_IteratorCheck.key();
            map_IteratorCheck.toBack();
        }
    }
    return true;
}

void CountResult::StartHook()
{
    m_pFilterVkCode->InitFilterHook();
}

void CountResult::StopHook()
{
    m_pFilterVkCode->CloseFilterHook();
}


bool CountResult::GetCheckData(const QString &str_Type,
                               QMap<int, bool> &map_KeyboardCheck)
{
    //从文件获取已记录好的数据进行对比
    QMap<int,STRUCT_KEYBOARDINFO> map_KeyboardData;
    DataAndInfoFile::ReadKeyboardConfig(str_Type, map_KeyboardData);

    if(map_KeyboardData.isEmpty())
        return false;

    bool b_HasVkCode = false;

    QMapIterator<int, STRUCT_KEYBOARDINFO> map_IteratorData(map_KeyboardData);{
        while (map_IteratorData.hasNext()){
            map_IteratorData.next();
            //遍历相同vk码，按键次数对比
            QMapIterator<int, int> map_IteratorVkCode(map_KeyboardVkCode);
            while (map_IteratorVkCode.hasNext()){
                map_IteratorVkCode.next();

                if(map_IteratorVkCode.key() == map_IteratorData.key()
                        /*&& map_IteratorVkCode.value() == 1*/){
                    b_HasVkCode = true;
                    map_IteratorVkCode.toBack();
                }
            }
            map_KeyboardCheck.insert(map_IteratorData.key(), b_HasVkCode);
            b_HasVkCode = false;
        }
    }
    return true;
}

void CountResult::GetCheckData(const QString &str_Type,
                                  QList<int> &list_Acquire,
                                  QMap<int, QList<KEYBOARDRESULTSTATE>> &map_Result,
                                  KEYTESTFEASIBILITY &Feasibility)
{
    //从文件获取已记录好的数据进行对比
    QList<int> list_Record;
    DataAndInfoFile::ReadRecordKeyTemplate(str_Type, list_Record);
    qDebug()<<list_Record;
    qDebug()<<list_Record.size();
    qDebug()<<list_Acquire;
    qDebug()<<list_Acquire.size();

    //对比获取数据比记录的数据，数据差别大不测试
    if(list_Acquire.size() - list_Record.size() > 15){
        Feasibility = CANNOTTEST;
        return;
    }

    int n_ListRecordSize = list_Record.size();
    int n_TNumber;

    //按顺序检测按键个数，进行数据校验
    for(int n_IndexT = 0;n_IndexT < n_ListRecordSize; n_IndexT++){
        n_TNumber = list_Acquire.count(list_Record.value(n_IndexT, 0));

        switch (n_TNumber) {
        case 0:{
            FindEqualToZero(n_IndexT, list_Record, list_Acquire, map_Result);
            break;
        }
        case 1:{
            FindEqualToOne(n_IndexT, list_Record, list_Acquire, map_Result);
            break;
        }
        default:{
            FindMoreThanOne(n_IndexT, n_TNumber, list_Record, list_Acquire, map_Result);
            break;
        }
        }
    }
}

void CountResult::FindEqualToZero(int &n_ListIndex,
                                  QList<int> &list_Record,
                                  QList<int> &list_Acquire,
                                  QMap<int, QList<KEYBOARDRESULTSTATE> > &map_Result)
{
    QList<KEYBOARDRESULTSTATE> list_State;
    //错按检测，当前位置获取数据后一位和当前位置记录数据后一位相同
    if(list_Acquire.value(n_ListIndex + 1, 0) == list_Record.value(n_ListIndex + 1, 0)){
        list_State = InsertDataToList(list_Record.value(n_ListIndex, 0), map_Result, WRONGKE_K);
        map_Result.insert(list_Record.value(n_ListIndex, 0), list_State);
        list_Acquire.removeAt(n_ListIndex);
        list_Acquire.insert(n_ListIndex, list_Record.value(n_ListIndex, 0));
    }
    //串码检测，当前位置获取数据后两位和当前位置记录数据后一位相同
    else if(list_Acquire.value(n_ListIndex + 2, 0) == list_Record.value(n_ListIndex + 1, 0)){
        list_State = InsertDataToList(list_Record.value(n_ListIndex, 0), map_Result, WRONGKE_K);
        //插入串码按键
        list_State = InsertDataToList(list_Record.value(n_ListIndex, 0), map_Result, SRRINGUARDS_K);
        map_Result.insert(list_Record.value(n_ListIndex, 0), list_State);
        list_Acquire.removeAt(n_ListIndex);
//        list_Acquire.removeAt(n_ListIndex + 1);
        list_Acquire.insert(n_ListIndex, list_Record.value(n_ListIndex, 0));
    }
    //漏按检测，当前位置获取数据和当前位置记录数据后一位相同
    else if((list_Acquire.value(n_ListIndex, 0) == list_Record.value(n_ListIndex + 1, 0))){
        list_State = InsertDataToList(list_Record.value(n_ListIndex, 0), map_Result, MISSKET_K);
        //插入漏按键
        map_Result.insert(list_Record.value(n_ListIndex, 0), list_State);
        //补缺
        list_Acquire.insert(n_ListIndex, list_Record.value(n_ListIndex, 0));
    }
    //无法检测，直接错按
    else{
        list_State = InsertDataToList(list_Record.value(n_ListIndex, 0), map_Result, MISSKET_K);
        //插入漏按键
        map_Result.insert(list_Record.value(n_ListIndex, 0), list_State);
        //补缺
        list_Acquire.insert(n_ListIndex, list_Record.value(n_ListIndex, 0));
    }
}

void CountResult::FindEqualToOne(int &n_ListIndex,
                                 QList<int> &list_Record,
                                 QList<int> &list_Acquire,
                                 QMap<int, QList<KEYBOARDRESULTSTATE>> &map_Result)
{
    if(list_Record.value(n_ListIndex, 0) == list_Acquire.value(n_ListIndex, 0)){
        //没问题
        QList<KEYBOARDRESULTSTATE> list_State = InsertDataToList(list_Record.value(n_ListIndex, 0), map_Result, NOERROR_K);
        map_Result.insert(list_Record.value(n_ListIndex, 0),  list_State);
    }
    else{
        int n_ErrorT = list_Record.value(n_ListIndex, 0);
        int n_ErrorIndex = list_Acquire.indexOf(n_ErrorT);
        QList<KEYBOARDRESULTSTATE> list_State;

        //串码检测，当前位置获取数据后一位和当前位置记录数据后一位相同
        if(list_Record.value(n_ListIndex + 1, 0) == list_Acquire.value(n_ErrorIndex + 1)){
            //插入串码按键
            list_State = InsertDataToList(list_Record.value(n_ListIndex - 1, 0), map_Result, SRRINGUARDS_K);
            map_Result.insert(list_Record.value(n_ListIndex - 1, 0),  list_State);

            for(int j = 0; j < (n_ErrorIndex - n_ListIndex);j++){
                list_Acquire.removeAt(n_ListIndex);
            }
        }
        //串码检测
        else if(list_Record.value(list_Record.indexOf(list_Acquire.value(n_ErrorIndex - 1, 0)) + 2, 0) == list_Acquire.value(n_ErrorIndex + 1)){
            list_State = InsertDataToList(list_Record.value(list_Record.indexOf(list_Acquire.value(n_ErrorIndex - 1, 0)) + 1, 0), map_Result, WRONGKE_K);
            map_Result.insert(list_Record.value(list_Record.indexOf(list_Acquire.value(n_ErrorIndex - 1, 0)) + 1, 0),  list_State);

            //插入漏按键
            list_State = InsertDataToList(list_Record.value(n_ListIndex, 0), map_Result, MISSKET_K);
            map_Result.insert(list_Record.value(n_ListIndex, 0),  list_State);
            //拿走
            list_Acquire.removeAt(n_ErrorIndex);
            //补
            list_Acquire.insert(n_ListIndex, list_Record.value(n_ListIndex, 0));
        }
        //无法检测，直接测试为错案
        else{
            list_State = InsertDataToList(list_Record.value(list_Record.indexOf(list_Acquire.value(n_ErrorIndex - 1, 0)) + 1, 0), map_Result, WRONGKE_K);
            map_Result.insert(list_Record.value(list_Record.indexOf(list_Acquire.value(n_ErrorIndex - 1, 0)) + 1, 0),  list_State);

            //插入漏按键
            list_State = InsertDataToList(list_Record.value(n_ListIndex, 0), map_Result, MISSKET_K);
            map_Result.insert(list_Record.value(n_ListIndex, 0),  list_State);

            //拿走
            list_Acquire.removeAt(n_ErrorIndex);
            //补
            list_Acquire.insert(n_ListIndex, list_Record.value(n_ListIndex, 0));
        }
    }
}

void CountResult::FindMoreThanOne(int &n_ListIndex,
                                  int &n_TNumber,
                                  QList<int> &list_Record,
                                  QList<int> &list_Acquire,
                                  QMap<int, QList<KEYBOARDRESULTSTATE> > &map_Result)
{
    if(list_Record.value(n_ListIndex - 1, 0) == list_Acquire.value(n_ListIndex - 1, 0)){
        QList<KEYBOARDRESULTSTATE> list_State = InsertDataToList(list_Record.value(n_ListIndex, 0), map_Result, NOERROR_K);
        map_Result.insert(list_Record.value(n_ListIndex, 0), list_State);

        int n_ErrorT = list_Record.value(n_ListIndex, 0);
        int n_ErrorIndex = list_Acquire.indexOf(n_ErrorT);

        for(int j = 0;j < n_TNumber - 1;j++){
            //重按检测
            if(list_Acquire.value(n_ErrorIndex, 0) == list_Acquire.value(n_ErrorIndex + 1, 0)){
                //重复按键
                list_State = InsertDataToList(list_Acquire.value(n_ErrorIndex, 0), map_Result, REPEATKEY_K);
                map_Result.insert(list_Acquire.value(n_ErrorIndex, 0),  list_State);
                //拿走
                list_Acquire.removeAt(n_ErrorIndex);
            }
            else{
                n_ErrorIndex = list_Acquire.indexOf(n_ErrorT, n_ErrorIndex + 1);

                //串码检测
                if(list_Record.value(list_Record.indexOf(list_Acquire.value(n_ErrorIndex - 1, 0)) + 1, 0) == list_Acquire.value(n_ErrorIndex + 1)){
                    //插入串码按键
                    list_State = InsertDataToList(list_Acquire.value(n_ErrorIndex - 1, 0), map_Result, SRRINGUARDS_K);
                    map_Result.insert(list_Acquire.value(n_ErrorIndex - 1, 0),  list_State);
                }
                //错按检测
                else if(list_Record.value(list_Record.indexOf(list_Acquire.value(n_ErrorIndex - 1, 0)) + 2, 0) == list_Acquire.value(n_ErrorIndex + 1)){
                    list_State = InsertDataToList(list_Record.value(list_Record.indexOf(list_Acquire.value(n_ErrorIndex - 1, 0)) + 1, 0), map_Result, WRONGKE_K);
                    map_Result.insert(list_Record.value(list_Record.indexOf(list_Acquire.value(n_ErrorIndex - 1, 0)) + 1, 0),  list_State);
                }
                //无法测试
                else{
                    list_State = InsertDataToList(list_Record.value(list_Record.indexOf(list_Acquire.value(n_ErrorIndex - 1, 0)) + 1, 0), map_Result, WRONGKE_K);
                    map_Result.insert(list_Record.value(list_Record.indexOf(list_Acquire.value(n_ErrorIndex - 1, 0)) + 1, 0),  list_State);
                }
                //拿走
                list_Acquire.removeAt(n_ErrorIndex);
            }
        }
    }
    else{
        int n_ErrorT = list_Record.value(n_ListIndex, 0);
        int n_ErrorIndex = list_Acquire.indexOf(n_ErrorT);

        QList<KEYBOARDRESULTSTATE> list_State;
        //串码检测
        if(list_Record.value(list_Record.indexOf(list_Acquire.value(n_ErrorIndex - 1, 0)) + 1, 0) == list_Acquire.value(n_ErrorIndex + 1)){
            //插入串码按键
            list_State = InsertDataToList(list_Acquire.value(n_ErrorIndex - 1, 0), map_Result, SRRINGUARDS_K);
            map_Result.insert(list_Acquire.value(n_ErrorIndex - 1, 0),  list_State);
        }
        //错按检测
        else if(list_Record.value(list_Record.indexOf(list_Acquire.value(n_ErrorIndex - 1, 0)) + 2, 0) == list_Acquire.value(n_ErrorIndex + 1)){
            list_State = InsertDataToList(list_Record.value(list_Record.indexOf(list_Acquire.value(n_ErrorIndex - 1, 0)) + 1, 0), map_Result, WRONGKE_K);
            map_Result.insert(list_Record.value(list_Record.indexOf(list_Acquire.value(n_ErrorIndex - 1, 0)) + 1, 0),  list_State);
        }

        list_Acquire.removeAt(n_ErrorIndex);
        list_Acquire.insert(n_ListIndex, list_Record.value(n_ListIndex, 0));

        for(int j = 0;j < n_TNumber - 1;j++){
            n_ErrorIndex = list_Acquire.indexOf(n_ErrorT, n_ErrorIndex + 1);
            //插入错按键
            //串码检测
            if(list_Record.value(list_Record.indexOf(list_Acquire.value(n_ErrorIndex - 1, 0)) + 1, 0) == list_Acquire.value(n_ErrorIndex + 1)){
                //插入串码按键
                list_State = InsertDataToList(list_Acquire.value(n_ErrorIndex - 1, 0), map_Result, SRRINGUARDS_K);
                map_Result.insert(list_Acquire.value(n_ErrorIndex - 1, 0),  list_State);
            }
            //错按检测
            else if(list_Record.value(list_Record.indexOf(list_Acquire.value(n_ErrorIndex - 1, 0)) + 2, 0) == list_Acquire.value(n_ErrorIndex + 1)){
                list_State = InsertDataToList(list_Record.value(list_Record.indexOf(list_Acquire.value(n_ErrorIndex - 1, 0)) + 1, 0), map_Result, WRONGKE_K);
                map_Result.insert(list_Record.value(list_Record.indexOf(list_Acquire.value(n_ErrorIndex - 1, 0))),  list_State);
            }
            //无法检测
            else{
                list_State = InsertDataToList(list_Record.value(list_Record.indexOf(list_Acquire.value(n_ErrorIndex - 1, 0)) + 1, 0), map_Result, WRONGKE_K);
                map_Result.insert(list_Record.value(list_Record.indexOf(list_Acquire.value(n_ErrorIndex - 1, 0)) + 1, 0),  list_State);
            }

            list_Acquire.removeAt(n_ErrorIndex);
        }
    }
}

QList<KEYBOARDRESULTSTATE> CountResult::InsertDataToList(int n_ListT,
                                                   QMap<int, QList<KEYBOARDRESULTSTATE> > &map_Result,
                                                   const KEYBOARDRESULTSTATE &k_State)
{
    //键盘测试状态map容器，NOERROR_K状态和其他状态互斥
    if(map_Result.contains(n_ListT)){
        QList<KEYBOARDRESULTSTATE> list_State;
        list_State = map_Result.value(n_ListT);

        if(list_State.contains(k_State)){
            if(list_State.size() > 1){
                if(list_State.contains(NOERROR_K)){
                    list_State.removeAll(NOERROR_K);
                }
            }
            return list_State;
        }
        else{
            list_State.append(k_State);
            if(list_State.size() > 1){
                if(list_State.contains(NOERROR_K)){
                    list_State.removeAll(NOERROR_K);
                }
            }
            return list_State;
        }
    }
    else{
        QList<KEYBOARDRESULTSTATE> list_State;
        list_State.append(k_State);
        return list_State;
    }
}

void CountResult::slot_UpVkCode(int n_VkCode)
{
    qDebug()<<"get" << n_VkCode;

    int n_Time = 1;
    if(map_KeyboardVkCode.contains(n_VkCode))
        n_Time = map_KeyboardVkCode.value(n_VkCode) + 1;

    map_KeyboardVkCode.insert(n_VkCode, n_Time);
    list_AcquireVkCode.append(n_VkCode);
}
