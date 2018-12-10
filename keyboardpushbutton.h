#ifndef KEYBOARDPUSHBUTTON_H
#define KEYBOARDPUSHBUTTON_H
#include <QPushButton>
#include "golab_define.h"

class KeyboardPushButton : public QPushButton
{
    Q_OBJECT
public:
    KeyboardPushButton(QPushButton *parent = 0);

    //设置pushbutton样式表
    void SetBtnQss(QPushButton *btn,int radius, QString normalColor, QString normalTextColor, QString normalBackgroundPixmap,
                   QString hoverColor, QString hoverTextColor, QString hoverBackgroundPixmap,
                   QString pressedColor, QString pressedTextColor,QString pressedBackgroundPixmap);
    //状态显示
    void StateShow(const KEYBOARDSTATE &state);
    //设置键盘VK码
    void SetVkCode(const int &n_VkCode);
    //获取键盘VK码
    int GetVkCode();
    //设置控件text
    void SetKeyText(const QString &str_KeyText);
    //获取控件text
    QString GetKeyText();

private:
    int m_nVkCode;          //控件vk码
    QString m_strKeyText;   //控件text
};

#endif // KEYBOARDPUSHBUTTON_H
