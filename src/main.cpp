#include <windows.h>
#include <iostream>

int width = 600, height = 600;

void DrawGrid(HDC hdc, int n) {
    int cellWidth = width / n;
    int cellHeight = height / n;

    // Рисование вертикальных линий
    for (int i = 1; i < n; ++i) {
        int x = i * cellWidth;
        MoveToEx(hdc, x, 0, nullptr);
        LineTo(hdc, x, height);
    }

    // Рисование горизонтальных линий
    for (int j = 1; j < n; ++j) {
        int y = j * cellHeight;
        MoveToEx(hdc, 0, y, nullptr);
        LineTo(hdc, width, y);
    }
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

            HPEN bluePen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
            HPEN redPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));

            FillRect(hdc, &ps.rcPaint, (HBRUSH) (bluePen));

            SelectObject(hdc, redPen);
            char* nValue = getenv("N");
            int N;
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

            EndPaint(hwnd, &ps);
            break;
        }
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

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
