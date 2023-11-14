#include <windows.h>
#include <Windowsx.h>
#include <iostream>
#include <vector>
#include <wingdi.h>

int width = 600, height = 600;
int N;
static COLORREF backgroundColor = RGB(0, 0, 255);

#define WM_DRAW_CIRCLE (WM_USER + 1)
#define WM_DRAW_X (WM_USER + 2)

void DrawGrid(HDC hdc, int n) {
    int cellWidth = width / n;
    int cellHeight = height / n;

    for (int i = 1; i < n; ++i) {
        int x = i * cellWidth;
        MoveToEx(hdc, x, 0, nullptr);
        LineTo(hdc, x, height);
    }

    for (int j = 1; j < n; ++j) {
        int y = j * cellHeight;
        MoveToEx(hdc, 0, y, nullptr);
        LineTo(hdc, width, y);
    }
}

void DrawCircle(HDC hdc, int row, int col) {
    Ellipse(hdc, width/N*row - width/N + (width / 40), height/N*col - width/N + (width / 40), width/N*row - (width / 40), height/N*col - (width / 40));
}

void DrawX(HDC hdc, int row, int col) {

    MoveToEx(hdc, width/N*row - width/N + (width / 40), height/N*col - width/N + (width / 40), nullptr);
    LineTo(hdc, width/N*row - (width / 40), height/N*col - (width / 40));

    MoveToEx(hdc, width/N*row - width/N + (width / 40), height/N*col - (width / 40), nullptr);
    LineTo(hdc, width/N*row - (width / 40), height/N*col - width/N + (width / 40));
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            HPEN bluePen = CreatePen(PS_SOLID, 1, backgroundColor);
            HPEN redPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));

            FillRect(hdc, &ps.rcPaint, (HBRUSH) (bluePen));

            SelectObject(hdc, redPen);
            char* nValue = getenv("N");
            if (nValue != nullptr) {
                N = std::stoi(nValue);
                if (N < 1) {
                    N = 3;
                }
            }
            else {
                N = 3;
            }
            DrawGrid(hdc, N);

            DeleteObject(bluePen);
            DeleteObject(redPen);

            EndPaint(hwnd, &ps);
            ReleaseDC(hwnd, hdc);
            break;
        }
        case WM_LBUTTONDOWN: {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);

            int row;
            int col;

            for (int i = N; i >= 1; i--) //find col and row
            {
                if (x <= width/N*i) {
                    row = i;
                }
                if (y <= height/N*i) {
                    col = i;
                }
            }
            PostMessage(hwnd, WM_DRAW_CIRCLE, row, col);
            InvalidateRect(hwnd, nullptr, TRUE); //update window
            break;
        }
        case WM_RBUTTONDOWN: {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);

            int row;
            int col;

            for (int i = N; i >= 1; i--) //find col and row
            {
                if (x <= width/N*i) {
                    row = i;
                }
                if (y <= height/N*i) {
                    col = i;
                }
            }
            PostMessage(hwnd, WM_DRAW_X, row, col);
            InvalidateRect(hwnd, nullptr, TRUE); //update window
            break;
        }
        case WM_DRAW_CIRCLE: {

            int row = (int)wParam;
            int col = (int)lParam;

            std::cout << row << " " << col << " left" << std::endl;

            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            HBRUSH yellowBrush = CreateSolidBrush(RGB(255, 255, 0));

            SelectObject(hdc, yellowBrush);
            DrawCircle(hdc, row, col);

            DeleteObject(yellowBrush);

            EndPaint(hwnd, &ps);
            ReleaseDC(hwnd, hdc);
            break;
        }
        case WM_DRAW_X: {

            int row = (int)wParam;
            int col = (int)lParam;

            std::cout << row << " " << col << " right" << std::endl;

            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            HPEN greenPen = CreatePen(PS_SOLID, 5, RGB(0, 255, 0));

            SelectObject(hdc, greenPen);
            DrawX(hdc, row, col);

            DeleteObject(greenPen);

            EndPaint(hwnd, &ps);
            ReleaseDC(hwnd, hdc);
            break;
        }
        case WM_KEYDOWN: {
            if (wParam == VK_ESCAPE || (GetKeyState(VK_CONTROL) & 0x8000) && wParam == 'Q') {
                PostQuitMessage(0);
                return 0;
            }
            if ((GetKeyState(VK_CONTROL) & 0x8000) && wParam == 'C') {
                system("start C:/Windows/System32/notepad.exe");
            }
            if (wParam == VK_RETURN) {
                backgroundColor = RGB(rand() % 256, rand() % 256, rand() % 256);
                InvalidateRect(hwnd, nullptr, TRUE);
            }
            break;
        }
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    srand(time(0));
    const char CLASS_NAME[] = "MyWindowClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
            0,                      // Optional window styles
            CLASS_NAME,             // Window class
            "WinAPI App",           // Window text
            WS_OVERLAPPEDWINDOW,    // Window style
            CW_USEDEFAULT, CW_USEDEFAULT, width + 15, height + 39,   // Size and position
            nullptr,                   // Parent window
            nullptr,                   // Menu
            hInstance,              // Instance handle
            nullptr                    // Additional application data
    );

    if (hwnd == nullptr) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
