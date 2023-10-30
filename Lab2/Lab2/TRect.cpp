#include "TRect.hpp"

TRect::TRect(const std::string &text) : m_text(text)
{
}

TRect::TRect() : m_text{"Base text"}
{
}

const std::string &TRect::operator=(const std::string &_text)
{
    m_text = _text;
    
    return m_text;
}

const std::string &TRect::operator=(const TRect &other)
{
    m_text = other.m_text;
    return m_text;
}

int TRect::draw(HDC hdc, RECT& rect)
{
    // Получение метрик
    TEXTMETRIC tm;
    GetTextMetrics(hdc, &tm);
    auto fontHeight = tm.tmHeight;
    float fontWidth = tm.tmAveCharWidth;
    fontWidth += 1.05;
    // Просчитываем количество символов в строке, в зависимости от переданного нового прямоугольника самой ячейки и ширины шрифта
    int symbInLine = (rect.right - rect.left) / fontWidth;
    // Просчитываем количество строк
    auto lines = m_text.size() / symbInLine;
    // Цикл по всем строкам ии если полученная высота вмещается по высоте в ячейку
    for (int i = 0; i <= lines && fontHeight * (i + 1) < (rect.bottom - rect.top); ++i)
    {
        // Подстраивание прямоугольинка ячейки под текст
        RECT newRect = rect;
        newRect.bottom += fontHeight * i;
        newRect.top += fontHeight * i + 2;
        // Отрисовка текста
        DrawText(hdc, m_text.substr(symbInLine * i, symbInLine).c_str(), -1, &newRect, DT_CENTER);
    }

    return fontHeight * lines;
}

int TRect::calculateNewTextHeight(HDC hdc, RECT& rect)
{
    TEXTMETRIC tm;
    GetTextMetrics(hdc, &tm);
    auto fontHeight = tm.tmHeight;
    float fontWidth = tm.tmAveCharWidth;
    fontWidth += 1.05;
    auto symbInLine = (rect.right - rect.left) / fontWidth;
    float lines = max(m_text.size() / symbInLine + 2, 1.2f);

    rect.bottom = (float)fontHeight * lines;
    return rect.bottom;
}
