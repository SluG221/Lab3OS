#include <windows.h>
#include <Windowsx.h>
#include <iostream>
#include <vector>
#include <fstream>

struct WindowConfig {
    int width;
    int height;
    int N;
    int bgR;
    int bgG;
    int bgB;
    int gridR;
    int gridG;
    int gridB;
};

WindowConfig DefaultConfig{
    600,
    600,
    3,
    0,
    0,
    255,
    255,
    0,
    0
};

WindowConfig config{};

int width, height;
int N, gridR, gridG, gridB;
static COLORREF backgroundColor;
static COLORREF gridColor;
static int scrollDelta = 0;
std::vector<std::vector<int>> matrix;

void LoadFromConfig(WindowConfig cfg) {
    width = cfg.width;
    height = cfg.height;
    N = cfg.N;
    backgroundColor = RGB(cfg.bgR, cfg.bgG, cfg.bgB);
    gridR = cfg.gridR;
    gridG = cfg.gridG;
    gridB = cfg.gridB;
    gridColor = RGB(gridR, gridG, gridB);
}

void SaveToStream(WindowConfig cfg) {
    cfg = {
            width,
            height,
            N,
            GetRValue(backgroundColor),
            GetGValue(backgroundColor),
            GetBValue(backgroundColor),
            gridR,
            gridG,
            gridB
    };
    std::ofstream file("WindowConfigStream.txt");
    if (file.is_open()) {
        file << cfg.width << "\n" << cfg.height << "\n" << cfg.N << "\n" << cfg.bgR << "\n" << cfg.bgG << "\n" << cfg.bgB << "\n" << cfg.gridR << "\n" << cfg.gridG << "\n" << cfg.gridB;
        file.close();
    }
}

WindowConfig LoadFromStream() {
    WindowConfig cfg{};
    std::ifstream file("WindowConfigStream.txt");
    if (file.is_open()) {
        file >> cfg.width >> cfg.height >> cfg.N >> cfg.bgR >> cfg.bgG >> cfg.bgB >> cfg.gridR >> cfg.gridG >> cfg.gridB;
        file.close();
    }
    else {
        return DefaultConfig;
    }
    return cfg;
}

void SaveToFile(WindowConfig cfg) {
    cfg = {
            width,
            height,
            N,
            GetRValue(backgroundColor),
            GetGValue(backgroundColor),
            GetBValue(backgroundColor),
            gridR,
            gridG,
            gridB
    };
    FILE *file;
    file = std::fopen("WindowConfigFile.txt", "w");
    fprintf(file, "%d\n", cfg.width);
    fprintf(file, "%d\n", cfg.height);
    fprintf(file, "%d\n", cfg.N);
    fprintf(file, "%d\n", cfg.bgR);
    fprintf(file, "%d\n", cfg.bgG);
    fprintf(file, "%d\n", cfg.bgB);
    fprintf(file, "%d\n", cfg.gridR);
    fprintf(file, "%d\n", cfg.gridG);
    fprintf(file, "%d\n", cfg.gridB);
    fclose(file);
}

WindowConfig LoadFromFile() {
    WindowConfig cfg{};
    FILE *file;
    file = std::fopen("WindowConfigFile.txt", "r");
    if (file != nullptr) {
        fscanf(file, "%d", &cfg.width);
        fscanf(file, "%d", &cfg.height);
        fscanf(file, "%d", &cfg.N);
        fscanf(file, "%d", &cfg.bgR);
        fscanf(file, "%d", &cfg.bgG);
        fscanf(file, "%d", &cfg.bgB);
        fscanf(file, "%d", &cfg.gridR);
        fscanf(file, "%d", &cfg.gridG);
        fscanf(file, "%d", &cfg.gridB);
        fclose(file);
    }
    else {
        return DefaultConfig;
    }
    return cfg;
}

void SaveToWinAPI(WindowConfig cfg) {
    cfg = {
            width,
            height,
            N,
            GetRValue(backgroundColor),
            GetGValue(backgroundColor),
            GetBValue(backgroundColor),
            gridR,
            gridG,
            gridB
    };
    HANDLE hFile = CreateFile("WindowConfigWinAPI.txt", GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile != INVALID_HANDLE_VALUE) {
        DWORD bytesWritten;
        WriteFile(hFile, &cfg, sizeof(WindowConfig), &bytesWritten, nullptr);
        CloseHandle(hFile);
    }
}

WindowConfig LoadFromWinAPI() {
    WindowConfig cfg {};
    HANDLE hFile = CreateFile("WindowConfigWinAPI.txt", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile != INVALID_HANDLE_VALUE) {
        DWORD bytesRead;
        ReadFile(hFile, &cfg, sizeof(WindowConfig), &bytesRead, nullptr);
        CloseHandle(hFile);
    }
    else {
        return DefaultConfig;
    }
    return cfg;
}

void SaveToFileMapping(WindowConfig cfg) {
    cfg = {
            width,
            height,
            N,
            GetRValue(backgroundColor),
            GetGValue(backgroundColor),
            GetBValue(backgroundColor),
            gridR,
            gridG,
            gridB
    };
    HANDLE hFileMapping = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, sizeof(WindowConfig), "WindowConfigFileMapping");
    if (hFileMapping != nullptr) {
        WindowConfig* mappedData = static_cast<WindowConfig*>(MapViewOfFile(hFileMapping, FILE_MAP_WRITE, 0, 0, sizeof(WindowConfig)));
        if (mappedData != nullptr) {
            *mappedData = cfg;
            UnmapViewOfFile(mappedData);
        }
        CloseHandle(hFileMapping);
    }
}

WindowConfig LoadFromFileMapping() {
    WindowConfig cfg = {0, 0,0,0,0,0,0,0,0};
    HANDLE hFileMapping = OpenFileMapping(FILE_MAP_READ, FALSE, "WindowConfigFileMapping");
    if (hFileMapping != nullptr) {
        WindowConfig* mappedData = static_cast<WindowConfig*>(MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, sizeof(WindowConfig)));
        if (mappedData != nullptr) {
            cfg = *mappedData;
            UnmapViewOfFile(mappedData);
        }
        else {
            return DefaultConfig;
        }
        CloseHandle(hFileMapping);
    }
    else {
        return DefaultConfig;
    }
    return cfg;
}

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
                    N = config.N;
                }
            }
            else {
                N = config.N;
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
                if (gridR == 255 && gridG == 0 && gridB != 0)
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

    std::string CfgSave;
    char* nValue = getenv("CfgSave");
    if (nValue != nullptr) {
        CfgSave = nValue;
    }

    if (CfgSave == "fstream") {
        std::cout << "[INFO] - Config loaded from fstream" << std::endl;
        config = LoadFromStream();
    }
    else if (CfgSave == "winapi") {
        std::cout << "[INFO] - Config loaded from WinAPI" << std::endl;
        config = LoadFromWinAPI();
    }
    else if (CfgSave == "filemapping") {
        std::cout << "[INFO] - Config loaded from filemapping" << std::endl;
        config = LoadFromFileMapping();
    }
    else {
        std::cout << "[INFO] - Config loaded from file" << std::endl;
        config = LoadFromFile();
    }

    LoadFromConfig(config);
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

    if (CfgSave == "fstream") {
        SaveToStream(config);
    }
    else if (CfgSave == "winapi") {
        SaveToWinAPI(config);
    }
    else if (CfgSave == "filemapping") {
        SaveToFileMapping(config);
    }
    else {
        SaveToFile(config);
    }

    return 0;
}
