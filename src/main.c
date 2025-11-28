#define UNICODE
#define _UNICODE
#define COBJMACROS

#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <stdio.h>

// Global variables
ID2D1Factory *pD2DFactory = NULL;
ID2D1HwndRenderTarget *pRenderTarget = NULL;
ID2D1SolidColorBrush *pBrush = NULL;
IDWriteFactory *pDWriteFactory = NULL;
IDWriteTextFormat *pTextFormat = NULL;


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HRESULT CreateDeviceIndependentResources(void);

HRESULT CreateDeviceResources(HWND hwnd);
void DiscardDeviceResources(void);

void OnPaint(HWND hwnd);
void OnResize(HWND hwnd);

// TODO: Add error handling popup (look perplexity on how)
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    (void)hPrevInstance;
    (void)pCmdLine;
    
    // Initialize COM
    CoInitialize(NULL);
    
    HRESULT hr = CreateDeviceIndependentResources();
    if (FAILED(hr)) {
        CoUninitialize();
        return 1;
    }
    
    const wchar_t CLASS_NAME[] = L"DirectWriteWindowClass";
    
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    
    RegisterClass(&wc);
    
    HWND hwnd = CreateWindow(
        CLASS_NAME,
        L"DirectWrite Example",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL
    );
    
    if (hwnd == NULL) {
        CoUninitialize();
        return 1;
    }
    
    ShowWindow(hwnd, nCmdShow);
    
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    CoUninitialize();
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
        case WM_PAINT:
            OnPaint(hwnd);
            return 0;
            
        case WM_SIZE:
            OnResize(hwnd);
            return 0;
            
        case WM_DESTROY:
            DiscardDeviceResources();
            if (pTextFormat) IDWriteTextFormat_Release(pTextFormat);
            if (pDWriteFactory) IDWriteFactory_Release(pDWriteFactory);
            if (pD2DFactory) ID2D1Factory_Release(pD2DFactory);
            PostQuitMessage(0);
            return 0;
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

HRESULT CreateDeviceIndependentResources(void)
{
    HRESULT hr = S_OK;
    
    // Create D2D1 factory
    hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        &IID_ID2D1Factory,
        NULL,
        (void**)&pD2DFactory
    );
    
    if (FAILED(hr)) return hr;
    
    // Create DirectWrite factory
    hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        &IID_IDWriteFactory,
        (IUnknown**)&pDWriteFactory
    );
    
    if (FAILED(hr)) return hr;
    
    // Create text format
    hr = IDWriteFactory_CreateTextFormat(
        pDWriteFactory,
        L"Arial",
        NULL,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        10.0f,
        L"en-us",
        &pTextFormat
    );
    
    if (SUCCEEDED(hr)) {
        IDWriteTextFormat_SetTextAlignment(pTextFormat, DWRITE_TEXT_ALIGNMENT_CENTER);
        IDWriteTextFormat_SetParagraphAlignment(pTextFormat, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }
    
    return hr;
}

HRESULT CreateDeviceResources(HWND hwnd)
{
    if (pRenderTarget != NULL) {
        return S_OK;
    }
    
    RECT rc;
    GetClientRect(hwnd, &rc);
    
    D2D1_SIZE_U size = {
        rc.right - rc.left,
        rc.bottom - rc.top
    };
    
    D2D1_RENDER_TARGET_PROPERTIES props = {
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        {DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_UNKNOWN},
        0, 0,
        D2D1_RENDER_TARGET_USAGE_NONE,
        D2D1_FEATURE_LEVEL_DEFAULT
    };
    
    D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProps = {
        hwnd,
        size,
        D2D1_PRESENT_OPTIONS_NONE
    };
    
    HRESULT hr = ID2D1Factory_CreateHwndRenderTarget(
        pD2DFactory,
        &props,
        &hwndProps,
        &pRenderTarget
    );
    
    if (SUCCEEDED(hr)) {
        D2D1_COLOR_F color = {0.0f, 0.0f, 0.3f, 1.0f};
        hr = ID2D1HwndRenderTarget_CreateSolidColorBrush(
            pRenderTarget,
            &color,
            NULL,
            &pBrush
        );
    }
    
    return hr;
}

void DiscardDeviceResources(void)
{
    if (pBrush) {
        ID2D1SolidColorBrush_Release(pBrush);
        pBrush = NULL;
    }
    if (pRenderTarget) {
        ID2D1HwndRenderTarget_Release(pRenderTarget);
        pRenderTarget = NULL;
    }
}

void OnPaint(HWND hwnd)
{
    HRESULT hr = CreateDeviceResources(hwnd);
    
    if (SUCCEEDED(hr)) {
        PAINTSTRUCT ps;
        BeginPaint(hwnd, &ps);
        
        ID2D1HwndRenderTarget_BeginDraw(pRenderTarget);
        
        D2D1_COLOR_F clearColor = {.3f, .0f, .0f, 1.0f};
        ID2D1HwndRenderTarget_Clear(pRenderTarget, &clearColor);
        
        RECT rc;
        GetClientRect(hwnd, &rc);
        D2D1_RECT_F layoutRect = {
            0.0f,
            0.0f,
            (FLOAT)(rc.right - rc.left),
            (FLOAT)(rc.bottom - rc.top)
        };
        
        const WCHAR *text = L"Hello Oyku!";
        UINT32 textLength = (UINT32)wcslen(text);
        
        ID2D1HwndRenderTarget_DrawText(
            pRenderTarget,
            text,
            textLength,
            pTextFormat,
            &layoutRect,
            (ID2D1Brush*)pBrush,
            D2D1_DRAW_TEXT_OPTIONS_NONE,
            DWRITE_MEASURING_MODE_NATURAL
        );
        
        hr = ID2D1HwndRenderTarget_EndDraw(pRenderTarget, NULL, NULL);
        
        if (hr == D2DERR_RECREATE_TARGET) {
            DiscardDeviceResources();
        }
        
        EndPaint(hwnd, &ps);
    }
}


void OnResize(HWND hwnd)
{
    if (pRenderTarget) {
        RECT rc;
        GetClientRect(hwnd, &rc);
        D2D1_SIZE_U size = {
            rc.right - rc.left,
            rc.bottom - rc.top
        };
        ID2D1HwndRenderTarget_Resize(pRenderTarget, &size);
        InvalidateRect(hwnd, NULL, FALSE);
    }
}