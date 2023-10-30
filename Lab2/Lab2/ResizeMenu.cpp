#include "ResizeMenu.hpp"

#include <stdexcept>
#include <memory>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iterator>
#include <Windowsx.h>
#include <CommCtrl.h>
#include <mshtmcid.h>
#include <Windef.h>

ResizeMenu::ResizeMenu(HWND parent, const std::string& className, const std::string& label) : m_className{className}, m_label{ label }
{
    try
    {
        RECT rect;
        GetWindowRect(parent, &rect);
        this->m_parent = parent;
        // Создание главного окна
        m_wndHandle = CreateWindowEx(0, className.c_str(), label.c_str(),  WS_DLGFRAME, rect.right / 2 - 160, rect.bottom / 2 - 90, 300, 300,
        parent, nullptr, GetModuleHandle(nullptr), this);
        if (!m_wndHandle)
            throw std::runtime_error{"Failed to create window!"};

    }
    catch(const std::exception& e)
    {
        MessageBox(nullptr, e.what(), "ERROR", MB_OK | MB_ICONERROR);
        throw;
    }
    
}

HWND ResizeMenu::addButton(const std::string &label, const RECT &posSize, HMENU id)
{
    return CreateWindow("Button", label.c_str(), WS_VISIBLE | WS_CHILD, posSize.left, posSize.top, posSize.right, posSize.bottom, m_wndHandle, id, NULL, NULL);
}
HWND ResizeMenu::addEdit(const std::string &label, const RECT &posSize, HMENU id)
{
    return CreateWindow("Edit", label.c_str(), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, posSize.left, posSize.top, posSize.right, posSize.bottom, m_wndHandle, id, NULL, NULL);
}
HWND ResizeMenu::addLabel(const std::string &label, const RECT &posSize, HMENU id)
{
    return CreateWindow("Static", label.c_str(), WS_VISIBLE | WS_CHILD, posSize.left, posSize.top, posSize.right, posSize.bottom, m_wndHandle, id, NULL, NULL);
}

int ResizeMenu::run()
{
    MSG msg{};
    ShowWindow(m_wndHandle, SW_SHOW);

    while (GetMessage(&msg, m_wndHandle, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return (int)(msg.wParam);
}

LRESULT ResizeMenu::appProg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    ResizeMenu* pWindow;
    if(uMsg == WM_NCCREATE)
    {
        pWindow = (ResizeMenu*)(((CREATESTRUCTW*)lParam)->lpCreateParams);
        SetLastError(0);
        if(!SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pWindow))
        {
            if(GetLastError() != 0)
                return false;
        }
    }
    else
        pWindow = (ResizeMenu*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    
    if(pWindow)
    {
        pWindow->m_wndHandle = hwnd;
        return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT ResizeMenu::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {  
        case WM_CREATE:
        {
            m_close = addButton("Закрыть", {40, 150, 80, 30}, (HMENU)CLOSE);
            m_ok = addButton("Ok", {120, 150, 80, 30}, (HMENU)OK);

            m_editRow = addEdit("", {30, 40, 100, 30}, (HMENU)EDIT_ROW);
            addLabel("Строки", {30, 10, 100, 20}, 0);
            m_editCol = addEdit("", {30, 110, 100, 30}, (HMENU)EDIT_COL);
            addLabel("Колонки", {30, 80, 100, 20}, 0);
            return true;
        }
        case WM_COMMAND:
        {
            switch (wParam)
            {
            case OK:
            {
                char text[100];
                GetWindowText(m_editRow, text, 100);
                {
                    std::istringstream in(text);
                    in >> resRow;
                }
                GetWindowText(m_editCol, text, 100);
                {
                    std::istringstream in(text);
                    in >> resCol;
                }
            }
            case CLOSE:
                EnableWindow(m_parent, true);
                DestroyWindow(hwnd);                
                break;
            case EDIT_ROW:
            {
                
                break;
            }
            case EDIT_COL:
            {
                char text[100];
                GetWindowText(m_editRow, text, 100);
                break;
            }
            }
            break;
        }
        case WM_GETMINMAXINFO:
        {
            LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
            lpMMI->ptMinTrackSize.x = 300;
            lpMMI->ptMinTrackSize.y = 300;
            return 0;
        }
        case WM_DESTROY:
        {
            EnableWindow(m_parent, true);
            return 0;
        }
        case WM_SIZE:
        {
            m_widht = LOWORD(lParam);
            m_height = HIWORD(lParam);
            return 0;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void ResizeMenu::registerClass(const CHAR* className_)
{
    WNDCLASSEX wc = { sizeof(WNDCLASSEX) };

    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.lpfnWndProc   = ResizeMenu::appProg;
    wc.hInstance     = GetModuleHandleW(nullptr);
    wc.lpszClassName = className_;
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = nullptr;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    
    if(!RegisterClassEx(&wc))
        throw std::runtime_error{"Failed to create window class!"};
}

HWND ResizeMenu::getHandle() 
{
    return m_wndHandle; 
}
