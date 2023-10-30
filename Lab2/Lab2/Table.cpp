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
// Установка текста при инициализации
void Table::setText(const std::string &str)
{
    m_text = str;
    for (auto& textArr : m_matrixTexts)
        for (auto& text : textArr)
            text = str;
}
// Установка текста при перерисовке(в связке с Rect)
void Table::setText(const TRect &other)
{
    m_text = other.getStr();
    for (auto& textArr : m_matrixTexts)
        for (auto& text : textArr)
            text = other;
}
// Изменение прямоугольика таблицы
void Table::resize(RECT rect)
{
    m_tableRect = rect;
}
// Изменение размера таблицы
void Table::resize(int rowNum, int colNum)
{
    // Новые размеры
    m_rows = rowNum;
    m_cols = colNum;
    // Вызов метода ресайза самой матрицы ячеек
    m_cellsDimensions.resize(m_rows);
    // Так как каждая строка это вектор из столбцов, то ресайз и для них
    for(auto& cellArr : m_cellsDimensions) cellArr.resize(m_cols);
    // Аналогично для текста
    m_matrixTexts.resize(m_rows);
    for (auto& textArr : m_matrixTexts) textArr.resize(m_cols);
    // Установка текста во все новые ячейки
    setText(m_text);
}

void Table::resize(int rowNum, int colNum, RECT rect)
{
    resize(rowNum, colNum);
    resize(rect);
}

void Table::calcCellsSize(HDC hdc, int& fontSize)
{
    // Изменение размера шрифта в зависимости от размера окна(ширины)
    fontSize = (m_tableRect.right - m_tableRect.left) / m_cols / 15;
    // Просчитывание новой щирины ячейки таблицы
    auto cellWidth = (m_tableRect.right - m_tableRect.left) / m_cols;
    int prevRowH = 0;
    // Цикл по всем строкам
    for(int i = 0; i < m_rows; ++i)
    {
        int prevRowW = 0;
        int maxRowH = 0;
        // Цикл по всем столбцам
        for(int j = 0; j < m_cols; ++j)
        {
            // Сохранение текущей ячейки
            auto& cell = m_cellsDimensions[i][j];
            // Новая ячейка
            cell = RECT{prevRowW, prevRowH, prevRowW + cellWidth, prevRowH};
            // Просчитывание новой высоты текста
            auto textHeight = m_matrixTexts[i][j].calculateNewTextHeight(hdc, cell);
            // Блок уменьшения размера(до 300)
            if (maxRowH < textHeight)
                maxRowH = textHeight;

            prevRowW += cellWidth;
        }
        // Установка высоты
        for(auto& cell : m_cellsDimensions[i])
            cell.bottom = prevRowH + maxRowH;
        prevRowH += maxRowH;
    }
    // Изменение прямоугольника самой таблицы
    resize({m_tableRect.left, m_tableRect.top, m_tableRect.right, prevRowH});
}

// Рисование таблицы
void Table::draw(HDC hdc, int offset)
{
    // Ghj[jl gj dctv zxtqrfv
    for(int i = 0; i < m_rows; ++i)
        for(int j = 0; j < m_cols; ++j)
        {
            // Сохранение текущего размера
            RECT rect = m_cellsDimensions[i][j];
            // Новый размер включает сдвиг, если крутим колесиком, который прибавляется
            rect.bottom += offset;
            rect.top += offset;
            // Рисование нового прямоугольника ячейки
            Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
            // Рисование соотвествующего текста ячейки
            m_matrixTexts[i][j].draw(hdc, rect);
        }
}
