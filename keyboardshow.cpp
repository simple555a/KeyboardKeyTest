#include "keyboardshow.h"
#include "dataandinfofile.h"
#include <QDebug>
#include <QObject>

KeyboardShow::KeyboardShow(QWidget *parent)
    : QWidget(parent)
{
    m_pQThread = NULL;
//    m_pQThread = new QThread;
//    this->moveToThread(m_pQThread);
//    m_pQThread->start();

    m_pFilterVkCode = NULL;
    m_pFilterVkCode = FilterVkCode::GetInstance();
}

KeyboardShow::~KeyboardShow()
{
    if(!m_pQThread){
        m_pQThread->quit();
        m_pQThread->wait();
        delete m_pQThread;
        m_pQThread = NULL;
    }
}

void KeyboardShow::ConnectKeyInfo()
{
    connect(m_pFilterVkCode, SIGNAL(sig_FilterKeyUp(int)), this, SLOT(slot_SingleKeyUp(int)));
    connect(m_pFilterVkCode, SIGNAL(sig_FilterKeyDown(int)), this, SLOT(slot_SingleKeyDown(int)));
}

void KeyboardShow::DisconnectKeyInfo()
{
    disconnect(m_pFilterVkCode, SIGNAL(sig_FilterKeyUp(int)), this, SLOT(slot_SingleKeyUp(int)));
    disconnect(m_pFilterVkCode, SIGNAL(sig_FilterKeyDown(int)), this, SLOT(slot_SingleKeyDown(int)));
}

QWidget *KeyboardShow::CreatKeyboardShow(const QString &str_Type)
{
    if(!m_mapButtonInfo.isEmpty())
        m_mapButtonInfo.clear();

    QMap<int,STRUCT_KEYBOARDINFO> map_KeyboardData;
    DataAndInfoFile::ReadKeyboardConfig(str_Type, map_KeyboardData);

    if(map_KeyboardData.isEmpty())
        return NULL;

    QWidget *QWidgetKeyboard = new QWidget;
    int n_MaxX =0, n_MaxY = 0;

    QMapIterator<int, STRUCT_KEYBOARDINFO> map_Iterator(map_KeyboardData);
    while (map_Iterator.hasNext()){
        map_Iterator.next();

        KeyboardPushButton* Button = CreatPushButton(map_Iterator.value());
        Button->setParent(QWidgetKeyboard);
        Button->move(map_Iterator.value().struct_Rect.n_AX, map_Iterator.value().struct_Rect.n_AY);
        Button->setFixedSize(map_Iterator.value().struct_Rect.n_BX - map_Iterator.value().struct_Rect.n_AX,
                             map_Iterator.value().struct_Rect.n_BY - map_Iterator.value().struct_Rect.n_AY);

        n_MaxX = qMax(map_Iterator.value().struct_Rect.n_BX, n_MaxX);
        n_MaxY = qMax(map_Iterator.value().struct_Rect.n_BY, n_MaxY);
    }

    QWidgetKeyboard->setMinimumSize(n_MaxX, n_MaxY);

    return QWidgetKeyboard;
}

KeyboardPushButton *KeyboardShow::CreatPushButton(const STRUCT_KEYBOARDINFO &struct_RectInfo)
{
    KeyboardPushButton *Button = new KeyboardPushButton;
    Button->setText(struct_RectInfo.str_Name);
    Button->SetKeyText(struct_RectInfo.str_Name);
    Button->SetVkCode(struct_RectInfo.n_Vkcode);
    m_mapButtonInfo.insert(struct_RectInfo.n_Vkcode, Button);
    return Button;
}

void KeyboardShow::ResetKeyboard(QString str_Msg)
{
    StateKeyboard(RESET_k, str_Msg);
}

void KeyboardShow::PressKeyboard(QString str_Msg)
{
    StateKeyboard(PRESS_k, str_Msg);
}

void KeyboardShow::ReleaseKeyboard(QString str_Msg)
{
    StateKeyboard(RELEASE_k, str_Msg);
}

void KeyboardShow::ErrorKeyboard(QString str_Msg)
{
    StateKeyboard(ERROR_k, str_Msg);
}

bool KeyboardShow::ShowKeyboardState(const QMap<int, bool> &map_KeyboardCheck)
{
    QMapIterator<int, KeyboardPushButton*> map_IteratorButton(m_mapButtonInfo);
    while (map_IteratorButton.hasNext()){
        map_IteratorButton.next();

        QMapIterator<int, bool> map_IteratorCheck(map_KeyboardCheck);

        while (map_IteratorCheck.hasNext()){
            map_IteratorCheck.next();
            if(map_IteratorCheck.key() == map_IteratorButton.key()){
                if(map_IteratorCheck.value())
                    map_IteratorButton.value()->StateShow(RELEASE_k);
                else
                    map_IteratorButton.value()->StateShow(ERROR_k);

                map_IteratorCheck.toBack();
            }
        }
    }
    return true;
}

void KeyboardShow::ShowKeyboardState(QMap<int, QList<KEYBOARDRESULTSTATE> > map_KeyResultCheck)
{
    QString Info = "";
    QMapIterator<int, KeyboardPushButton*> map_IteratorButton(m_mapButtonInfo);
    while (map_IteratorButton.hasNext()){
        map_IteratorButton.next();

        //目前fn键检测不到，特殊处理显示,后期删除
//        if(map_IteratorButton.key() == 0xa6){
//            map_IteratorButton.value()->StateShow(RELEASE_k);
//            continue;
//        }

        QMapIterator<int, QList<KEYBOARDRESULTSTATE>> map_IteratorCheck(map_KeyResultCheck);
        while (map_IteratorCheck.hasNext()){
            map_IteratorCheck.next();
            if(map_IteratorCheck.key() == map_IteratorButton.key()){

                if(map_IteratorCheck.value().contains(NOERROR_K)){
                    map_IteratorButton.value()->StateShow(RELEASE_k);
                    continue;
                }

                if(map_IteratorCheck.value().contains(MISSKET_K)){
                    Info += QStringLiteral("漏/") ;
                }

                if(map_IteratorCheck.value().contains(REPEATKEY_K)){
                    Info += QStringLiteral("重/") ;
                }

                if(map_IteratorCheck.value().contains(WRONGKE_K)){
                    Info += QStringLiteral("错/");
                }

                if(map_IteratorCheck.value().contains(SRRINGUARDS_K)){
                    Info += QStringLiteral("串/");
                }

                map_IteratorButton.value()->StateShow(ERROR_k);
                map_IteratorButton.value()->setText(map_IteratorButton.value()->GetKeyText() + "\n" +  Info);
                Info = "";
            }
        }
    }
}

void KeyboardShow::StateKeyboard(const KEYBOARDSTATE &state, const QString &str_Msg)
{
    QMapIterator<int, KeyboardPushButton*> map_Iterator(m_mapButtonInfo);
    while (map_Iterator.hasNext()){
        map_Iterator.next();
        map_Iterator.value()->StateShow(state);
        map_Iterator.value()->setText(map_Iterator.value()->GetKeyText() + "\n" + str_Msg);
    }
}

void KeyboardShow::slot_SingleKeyDown(int n_VkCode)
{
    SingleKeyDown(n_VkCode);
}

void KeyboardShow::slot_SingleKeyUp(int n_VkCode)
{
    SingleKeyUp(n_VkCode);
}

void KeyboardShow::SingleKeyDown(int n_VkCode)
{
    QMapIterator<int, KeyboardPushButton*> map_Iterator(m_mapButtonInfo);
    while (map_Iterator.hasNext()){
        map_Iterator.next();
        if(map_Iterator.key() == n_VkCode){
            map_Iterator.value()->StateShow(PRESS_k);
            map_Iterator.toBack();
        }
    }
}

void KeyboardShow::SingleKeyUp(int n_VkCode)
{
    QMapIterator<int, KeyboardPushButton*> map_Iterator(m_mapButtonInfo);
    while (map_Iterator.hasNext()){
        map_Iterator.next();
        if(map_Iterator.key() == n_VkCode){
            map_Iterator.value()->StateShow(RELEASE_k);
            map_Iterator.toBack();
        }
    }
}

