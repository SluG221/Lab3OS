#include <windows.h>
#include <Windowsx.h>
#include <iostream>
#include <vector>

int width = 600, height = 600;
int N, gridR = 255, gridG = 0, gridB = 0;
static COLORREF backgroundColor = RGB(0, 0, 255);
static COLORREF gridColor = RGB(gridR, gridG, gridB);
static int scrollDelta = 0;
std::vector<std::vector<int>> matrix;

void ClearMatrix() {
    matrix.clear();
    for (int i = 0; i < N; i++) {
        std::vector<int> temp;
        for (int j = 0; j < N; j++) {
            temp.push_back(0);
        }
        matrix.push_back(temp);
    }
}

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

            HPEN bg = CreatePen(PS_SOLID, 1, backgroundColor);
            HPEN grid = CreatePen(PS_SOLID, 2, gridColor);
            HPEN blackPen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
            HPEN greenPen = CreatePen(PS_SOLID, 5, RGB(0, 255, 0));
            HBRUSH yellowBrush = CreateSolidBrush(RGB(255, 255, 0));

            FillRect(hdc, &ps.rcPaint, (HBRUSH) (bg));

            SelectObject(hdc, grid);
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

            if (matrix.empty()) {
                ClearMatrix();
            }

            DrawGrid(hdc, N);

            SelectObject(hdc, blackPen);
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    if (matrix[i][j] == 1) {
                        SelectObject(hdc, yellowBrush);
                        DrawCircle(hdc, i + 1, j + 1);
                        SelectObject(hdc, blackPen);
                    }
                    if (matrix[i][j] == 2) {
                        SelectObject(hdc, greenPen);
                        DrawX(hdc, i + 1, j + 1);
                        SelectObject(hdc, blackPen);
                    }
                }
            }

            DeleteObject(bg);
            DeleteObject(grid);
            DeleteObject(greenPen);
            DeleteObject(yellowBrush);

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

            if (matrix[row-1][col-1] == 0) {
                matrix[row-1][col-1] = 1;
                InvalidateRect(hwnd, nullptr, TRUE);
            }
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

            if (matrix[row-1][col-1] == 0) {
                matrix[row-1][col-1] = 2;
                InvalidateRect(hwnd, nullptr, TRUE);
            }
            break;
        }
        case WM_MOUSEWHEEL: {
            scrollDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            if (scrollDelta == WHEEL_DELTA) {
                if (gridR == 255 && gridG == 0)
                    gridB += 15;
                if (gridG == 0 && gridB == 255)
                    gridR -= 15;
                if (gridR == 0 && gridB == 255)
                    gridG += 15;
                if (gridG == 255 && gridR == 0)
                    gridB -= 15;
                if (gridG == 255 && gridB == 0)
                    gridR += 15;
                if (gridR == 255 && gridB == 0)
                    gridG -= 15;
            }
            else {
                if (gridR == 255 && gridG == 0)
                    gridB -= 15;
                if (gridG == 0 && gridB == 255)
                    gridR += 15;
                if (gridR == 0 && gridB == 255)
                    gridG -= 15;
                if (gridG == 255 && gridR == 0)
                    gridB += 15;
                if (gridG == 255 && gridB == 0)
                    gridR -= 15;
                if (gridR == 255 && gridB == 0)
                    gridG += 15;
            }
            gridColor = RGB(gridR, gridG, gridB);
            InvalidateRect(hwnd, nullptr, TRUE);
            break;
        }
        case WM_KEYDOWN: {
            if (wParam == VK_ESCAPE || (GetKeyState(VK_CONTROL) & 0x8000) && wParam == 'Q') {
                PostQuitMessage(0);
                return 0;
            }
            else if ((GetKeyState(VK_CONTROL) & 0x8000) && wParam == 'C') {
                system("start C:/Windows/System32/notepad.exe");
            }
            else if (wParam == VK_RETURN) {
                backgroundColor = RGB(rand() % 256, rand() % 256, rand() % 256);
                InvalidateRect(hwnd, nullptr, TRUE);
            }
            else if ((GetKeyState(VK_CONTROL) & 0x8000) && wParam == 'R') {
                ClearMatrix();
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
