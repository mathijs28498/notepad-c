#ifndef UNICODE
#define UNICODE
#endif

#define UNUSED(x) (void)(x)

#include <windows.h>
#include <stdio.h>
#include <wchar.h>
#include <dwrite.h>
#include <d2d1.h>

// IDWriteFactory *dWriteFactory;
// IDWriteTextFormat *textFormat;

// const wchar_t wszText;
// UINT32 textLen;

// ID2D1Factory *d2dFactory;
// ID2D1HwndRenderTarget *rt;
// ID2D1SolidColorBrush *blackBrush;


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
    UNUSED(hPrevInstance);
    UNUSED(lpCmdLine);

    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = {0};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,                   // Optional window styles.
        CLASS_NAME,          // Window class
        L"Text editor yes!", // Window text
        WS_OVERLAPPEDWINDOW, // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,      // Parent window
        NULL,      // Menu
        hInstance, // Instance handle
        NULL       // Additional application data
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {0};

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static size_t current_line_number = 0;
    static size_t current_char_number = 0;
    static WCHAR lines[][255] = {
        L"This is line 0",
        L"And then another",
        L"And then antoher",
        L"How's the weather?",
        L"How's the weather?",
        L"How's the weather?",
        L"How's the weather?",
        L"How's the weather?",
        L"How's the weather?",
        L"How's the weather?",
        L"How's the weather?",
        L"How's the weather?",
        L"How's the weather?",
        L"How's the weather?",
        L"How's the weather?",
        L"How's the weather?",
        L"How's the weather?",
        L"How's the weather?",
        L"How's the weather?",
        L"How's the weather?",
        L"How's the weather?",
    };

    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_KEYDOWN:
        wchar_t pressed_char = (wchar_t)wParam;
        printf("key down: 0x%llX\n", wParam);
        if (wParam == VK_DOWN)
        {
            if (current_line_number < ARRAYSIZE(lines) - 1)
                current_line_number++;
            // TODO: Only invalidate the area of the screen affected
            InvalidateRect(hwnd, NULL, TRUE);
        }
        if (wParam == VK_UP)
        {
            if (current_line_number > 0)
                current_line_number--;
            // TODO: Only invalidate the area of the screen affected
            InvalidateRect(hwnd, NULL, TRUE);
        }
        if (wParam == VK_LEFT)
        {
            if (current_char_number > 0)
                current_char_number--;
            // TODO: Only invalidate the area of the screen affected
            InvalidateRect(hwnd, NULL, TRUE);
        }
        if (wParam == VK_RIGHT)
        {
            if (current_char_number < wcslen(lines[current_line_number]))
                current_char_number++;
            // TODO: Only invalidate the area of the screen affected
            InvalidateRect(hwnd, NULL, TRUE);
        }

        if (current_char_number > wcslen(lines[current_line_number]))
        {
            current_char_number = wcslen(lines[current_line_number]);
        }

        if (wParam == VK_BACK && current_char_number > 0)
        {
            // TODO: Memory bound checks
            wmemmove(&lines[current_line_number][current_char_number] - 1,
                     &lines[current_line_number][current_char_number],
                     wcslen(lines[current_line_number]) - current_char_number + 1);

            current_char_number--;

            // TODO: Only invalidate the area of the screen affected
            InvalidateRect(hwnd, NULL, TRUE);
        }

        if (wParam == VK_DELETE && current_char_number < wcslen(lines[current_line_number]))
        {
            // TODO: Memory bound checks
            wmemmove(&lines[current_line_number][current_char_number],
                     &lines[current_line_number][current_char_number] + 1,
                     wcslen(lines[current_line_number]) - current_char_number + 1);

            // TODO: Only invalidate the area of the screen affected
            InvalidateRect(hwnd, NULL, TRUE);
        }

        if (!iswalnum(pressed_char) && pressed_char != L' ')
        {
            break;
        }

        if (!(GetKeyState(VK_SHIFT) & 0x8000))
        {
            pressed_char = towlower(pressed_char);
        }

        // TODO: Memory bound checks
        wmemmove(&lines[current_line_number][current_char_number] + 1,
                 &lines[current_line_number][current_char_number],
                 wcslen(lines[current_line_number]) - current_char_number + 1);

        lines[current_line_number][current_char_number] = pressed_char;
        current_char_number++;

        // TODO: Only invalidate the area of the screen affected
        InvalidateRect(hwnd, NULL, TRUE);
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Do painting
        static int iter = 0;
        printf("Doing some painting %d\n", iter++);

        HFONT hFont, hOldFont;

        int pointSize, fontHeight;

        // Calculate font height from point size
        pointSize = 11; // Your desired font size in points
        fontHeight = -MulDiv(pointSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);

        // Create the font
        hFont = CreateFont(
            fontHeight,                  // Height (negative for character height)
            0,                           // Width (0 = closest match)
            0,                           // Escapement (rotation)
            0,                           // Orientation
            FW_NORMAL,                   // Weight (FW_BOLD, FW_NORMAL, etc.)
            FALSE,                       // Italic
            FALSE,                       // Underline
            FALSE,                       // Strikeout
            DEFAULT_CHARSET,             // Character set
            OUT_DEFAULT_PRECIS,          // Output precision
            CLIP_DEFAULT_PRECIS,         // Clipping precision
            ANTIALIASED_QUALITY,         // Quality
            DEFAULT_PITCH | FF_DONTCARE, // Pitch and family
            TEXT("Arial")                // Font face name
        );

        // hFont = (HFONT)GetStockObject(ANSI_VAR_FONT);

        if ((hOldFont = (HFONT)SelectObject(hdc, hFont)))
        {
            for (int i = 0; i < ARRAYSIZE(lines); i++)
            {

                WCHAR line_number_str[3];
                swprintf_s(line_number_str, ARRAYSIZE(line_number_str), L"%d", i);

                SetTextColor(hdc, 0x00666666);
                TextOut(hdc, 5, 10 + i * 20, line_number_str, wcslen(line_number_str));

                SetTextColor(hdc, 0x00000000);
                const WCHAR *text = lines[i];
                TextOut(hdc, 35, 10 + i * 20, text, wcslen(text));
            }

            SelectObject(hdc, hOldFont);

            SIZE current_line_number_str_size;
            GetTextExtentPoint32(hdc, lines[current_line_number], current_char_number, &current_line_number_str_size);

            RECT line_location_rect;
            SetRect(&line_location_rect, 35 + current_line_number_str_size.cx, 10 + current_line_number * 20, 35 + current_line_number_str_size.cx + 1, 10 + (current_line_number + 1) * 20);
            // SetRect(&line_location_rect, 10, 10, 100, 100);
            FillRect(hdc, &line_location_rect, (HBRUSH)(COLOR_WINDOW + 2));
            // current_line_number_str_size->cx;
        }

        EndPaint(hwnd, &ps);
    }
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}