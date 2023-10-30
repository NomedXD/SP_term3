#pragma once

#include <string>
#include <Windows.h>

class TRect
{
private:
    std::string m_text;

public:
    TRect(const std::string& text);
    TRect();

    const std::string& operator=(const std::string& _text); 
    const std::string& operator=(const TRect& other); 
    const std::string& getStr() const { return m_text; }; 
    std::string& getStr() { return m_text; }; 

    int draw(HDC hdc, RECT& Rect);
    int calculateNewTextHeight(HDC hdc, RECT& Rect);
};