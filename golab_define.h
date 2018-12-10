#ifndef GOLAB_DEFINE_H
#define GOLAB_DEFINE_H
#include <QString>

//PLC信号描述
const QString PLC_FindUSB = "KBT.KONE.KO.PLC_FindUSB";
const QString PLC_Ready = "KBT.KONE.KO.PLC_Ready";
const QString PLC_Match_OK = "KBT.KONE.KO.PLC_Match_OK";
const QString PLC_Match_NG = "KBT.KONE.KO.PLC_Match_NG";
const QString PLC_StartKey = "KBT.KONE.KO.PLC_StartKey";
const QString PLC_Result_OK = "KBT.KONE.KO.PLC_Result_OK";
const QString PLC_Result_NG = "KBT.KONE.KO.PLC_Result_NG";
const QString PLC_FinishKey = "KBT.KONE.KO.PLC_FinishKey";

//lable状态显示颜色
const QString Color_ready = "rgb(170, 166, 160)";
const QString Color_run = "rgb(255, 242, 0)";
const QString Color_pass = "rgb(65, 242, 16)";
const QString Color_warning = "rgb(255, 0, 0)";

//PLC状态
enum PLCVALUERESULT
{
    Reset       = 0,
    NoFind      = 0,
    Find        = 1,
    NG          = 1,
    OK          = 1,
    Ready       = 1,
    StartKey    = 1,
    FinishKey   = 1,
};

//设备类型
enum WIREDORWIRLESS
{
    WIRED = 0,
    WIRLESS,
};

//设备测试可能性
enum KEYTESTFEASIBILITY
{
    CANTEST = 0,
    CANNOTTEST,
};

//按键结果状态
enum KEYBOARDRESULTSTATE
{
    NOERROR_K = 0,
    MISSKET_K,
    REPEATKEY_K,
    WRONGKE_K,
    SRRINGUARDS_K,
};

//按键状态描述
enum KEYBOARDSTATE{
    RESET_k = 0,
    PRESS_k,
    RELEASE_k,
    ERROR_k
};

//按键测试状态描述
enum WORKINGSTATE
{
    READY_S = 0,
    RUN_S,
    MATCH_OK_S,
    MATCH_NG_S,
    PASS_S,
    ERROR_S
};

//按键pushbutton大小
struct STRUCT_CRECT{
    int n_AX;
    int n_AY;
    int n_BX;
    int n_BY;
};

//按键控件信息
struct STRUCT_KEYBOARDINFO
{
    QString str_Caption;
    int n_Vkcode;
    QString str_Name;
    QString str_KState;
    STRUCT_CRECT struct_Rect;
};


#endif // GOLAB_DEFINE_H
