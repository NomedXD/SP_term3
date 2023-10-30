#pragma once

#include "TRect.hpp"
#include <vector>

class Table
{
private:
    int m_rows;
    int m_cols;
    RECT m_tableRect;

    std::vector<std::vector<TRect>> m_matrixTexts;
    std::vector<std::vector<RECT>> m_cellsDimensions;
    std::string m_text;

public:
    Table(int rowNum, int colNum, RECT clientRect);
    Table();

    void setText(const std::string& str);
    void setText(const TRect& text);

    inline void resize(RECT clientRect);
    inline void resize(int rows, int cols);
    void resize(int rows, int cols, RECT clientRect);
    void calcCellsSize(HDC hdc, int& fontSize);
    RECT getRect() { return m_tableRect; };

    void draw(HDC hdc, int offset);
};