#ifndef KEYBOARDVIRTUALKEY_H
#define KEYBOARDVIRTUALKEY_H
#include <stdint.h>
#include <QObject>
#include "golab_define.h"
#include <QThread>

class KeyboardVirtualKey : public QThread
{
    Q_OBJECT
public:
    KeyboardVirtualKey(QObject* parent = 0);
    ~KeyboardVirtualKey();
    //设置要按的按键
    void SetVirtualKey(uint16_t us_Key);

private:
    void FunctionKeyLock(uint16_t us_Key);
    void PressKeyboard(uint16_t us_Key);

protected:
    void run();

private:
    uint16_t m_usKeyData;
};

#endif // KEYBOARDVIRTUALKEY_H
