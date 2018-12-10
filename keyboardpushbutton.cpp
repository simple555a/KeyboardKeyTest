#include "keyboardpushbutton.h"

const QString btn_back_color =
        "qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0 #4D9DFB, stop:1 #85D0FE)";


KeyboardPushButton::KeyboardPushButton(QPushButton *parent)
    :QPushButton(parent)
{
    m_nVkCode = 0;
    m_strKeyText = "";
}

void KeyboardPushButton::SetBtnQss(QPushButton *btn, int radius, QString normalColor,
                                   QString normalTextColor, QString normalBackgroundPixmap,
                                   QString hoverColor, QString hoverTextColor,
                                   QString hoverBackgroundPixmap, QString pressedColor,
                                   QString pressedTextColor, QString pressedBackgroundPixmap)
{
    QStringList qss;

    qss.append(QString("QPushButton{font-size: 12px;line-height: 27.2px;outline:none;"
                       "border-style:none;border:none;border-radius:%4px;color:%1;"
                       "background:%2;border-image: url(%3);}")
               .arg(normalTextColor)
               .arg(normalColor)
               .arg(normalBackgroundPixmap)
               .arg(radius));
    qss.append(QString("QPushButton:hover{color:%1;background:%2;border-image: url(%3);}")
               .arg(hoverTextColor)
               .arg(hoverColor)
               .arg(hoverBackgroundPixmap));
    qss.append(QString("QPushButton:pressed{color:%1;background:%2;border-image: url(%3);}")
               .arg(pressedTextColor)
               .arg(pressedColor)
               .arg(pressedBackgroundPixmap));
    btn->setStyleSheet(qss.join(""));
}

void KeyboardPushButton::StateShow(const KEYBOARDSTATE &state)
{
    switch (state) {
    case RESET_k:
        SetBtnQss(this, 0,"#63bcff", "#132135",NULL,
                  "#63bcff", "#132135",NULL,
                  btn_back_color, "#132135",NULL);
        break;
    case PRESS_k:
        SetBtnQss(this, 0,"#ffff00", "#132135",NULL,
                  "#ffff00", "#132135",NULL,
                  btn_back_color, "#132135",NULL);
        break;
    case RELEASE_k:
        SetBtnQss(this, 0,"#00ff00", "#132135",NULL,
                  "#00ff00", "#132135",NULL,
                  btn_back_color, "#00ff00",NULL);
        break;
    case ERROR_k:
        SetBtnQss(this, 0,"#ff5500", "#132135",NULL,
                  "#ff5500", "#132135",NULL,
                  btn_back_color, "#132135",NULL);
        break;
    default:
        break;
    }
}

void KeyboardPushButton::SetVkCode(const int &n_VkCode)
{
    m_nVkCode = n_VkCode;
}

int KeyboardPushButton::GetVkCode()
{
    return m_nVkCode;
}

void KeyboardPushButton::SetKeyText(const QString &str_KeyText)
{
    m_strKeyText = str_KeyText;
}

QString KeyboardPushButton::GetKeyText()
{
    return m_strKeyText;
}
