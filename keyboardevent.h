#ifndef KEYBOARDEVENT_H
#define KEYBOARDEVENT_H
#include <QWidget>

class KeyboardEvent : public QWidget
{
    Q_OBJECT
public:
    KeyboardEvent(QWidget *parent = 0);
    void SetEventSwitch(const bool &b_Switch);
    bool GetEventSwitch();

signals:
    void sig_PressNativeScanCode(quint32);
    void sig_PressNativeVirtualKey(quint32);

    void sig_ReleaseNativeScanCode(quint32);
    void sig_ReleaseNativeVirtualKey(quint32);

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    bool m_bEventSwitch;
};

#endif // KEYBOARDEVENT_H
