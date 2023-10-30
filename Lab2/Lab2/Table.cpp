#include "Table.hpp"

#include <algorithm>
#include <utility>

Table::Table(int rowNum, int colNum, RECT rect)
{
    resize(rowNum, colNum, rect);
    setText("");
}

Table::Table() : Table(1, 1, {0, 0, 0, 0})
{
}

void Table::setText(const std::string &str)
{
    m_text = str;
}

void Table::setText(const TRect &other)
{
    m_text = other.getStr();
}

void Table::resize(RECT rect)
{
    m_tableRect = rect;
}

void Table::resize(int rowNum, int colNum)
{
    m_rows = rowNum;
    m_cols = colNum;

    m_cellsDimensions.resize(m_rows);
    for(auto& cellArr : m_cellsDimensions) cellArr.resize(m_cols);

    setText(m_text);
}

void Table::resize(int rowNum, int colNum, RECT rect)
{
    resize(rowNum, colNum);
    resize(rect);
}

void Table::calcCellsSize(HDC hdc)
{
    auto cellWidth = (m_tableRect.right - m_tableRect.left) / m_cols;
    int prevRowH = 0;
    for(int i = 0; i < m_rows; ++i)
    {
        int prevRowW = 0;
        int maxRowH = 0;
        for(int j = 0; j < m_cols; ++j)
        {
            auto& cell = m_cellsDimensions[i][j];
            cell = RECT{prevRowW, prevRowH, prevRowW + cellWidth, prevRowH};

            prevRowW += cellWidth;
        }
        for(auto& cell : m_cellsDimensions[i])
            cell.bottom = prevRowH + maxRowH;
        prevRowH += maxRowH;
    }
    resize({m_tableRect.left, m_tableRect.top, m_tableRect.right, prevRowH});
}

void Table::draw(HDC hdc, int offset)
{
    for(int i = 0; i < m_rows; ++i)
        for(int j = 0; j < m_cols; ++j)
        {
            RECT rect = m_cellsDimensions[i][j];
            rect.bottom += offset;
            rect.top += offset;
            Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
        }
}
