// Sandbox.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
//#include "stdafx.h"
#include "framework.h"
#include "EntryPoint.h"
#include "Sandbox.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
Sandbox::Sandbox* g_pApp = nullptr;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.
#ifdef _DEBUG
    AllocConsole();
#endif

    g_pApp = new Sandbox::Sandbox;
    assert(g_pApp);

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SANDBOX, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SANDBOX));

    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    // 기본 메시지 루프입니다:
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        else
        {
            g_pApp->Run();
        }
    }

    if (g_pApp)
    {
        delete g_pApp;
        g_pApp = nullptr;
    }

#ifdef _DEBUG
    system("pause");
    FreeConsole();
#endif

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SANDBOX));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;   // 메뉴바 제거
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   bool bFullScreen = false;
   bool bBorderless = false;
   int posX         = CW_USEDEFAULT;
   int posY         = 0;
   int width        = CW_USEDEFAULT;
   int height       = 0;

   // ini 파일 로드
   width = 800;
   height = 600;

   HWND hWnd = NULL;

   hWnd = CreateWindowEx(
       WS_EX_APPWINDOW,
       szWindowClass,
       szTitle,
       bBorderless ? WS_POPUP : WS_OVERLAPPEDWINDOW,
       posX, posY,
       width, height,
       NULL,
       NULL,
       hInstance,
       NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   g_pApp->SetWindow(hWnd, bFullScreen);

   // 해상도를 맞추려면 크기를 변경해야 한다.

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // 몇가지 메시지는 app의 초기화 여부를 확인한 후 수행되어야 한다.
    // 이와 관련된 코드를 말끔하게 다듬자.
    switch (message)
    {
    case WM_SIZE:
        // 일단은 직접 보내보자.
        {        
        if (!g_pApp->IsInitialized()) return 0;
        auto pGraphicsDevice = Dive::Renderer::GetInstance().GetGraphicsDevice();
        if (pGraphicsDevice->IsInitialized())
        {
            unsigned int width = lParam & 0xFFFF;
            unsigned int height = (lParam >> 16) & 0xFFFF;
            APP_TRACE("Call Resize Resolution : {0:d} x {1:d}", width, height);
            pGraphicsDevice->ResizeBuffers(width, height);
        }
        }
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_INPUT:
        {
            if (g_pApp->IsInitialized())
            {
                Dive::Input::GetInstance().ParseMessage(lParam);
            }
        }
        break;
    case WM_KEYDOWN:
        {
        auto pGraphicsDevice = Dive::Renderer::GetInstance().GetGraphicsDevice();
        if (pGraphicsDevice->IsInitialized())
        {
            switch (wParam)
            {
            case VK_F1:
            {
                pGraphicsDevice->ResizeTarget(800, 600);
                break;
            }
            case VK_F2:
            {
                pGraphicsDevice->ResizeTarget(1600, 900);
                break;
            }
            case VK_F3:
            {
                pGraphicsDevice->ResizeTarget(400, 300);
                break;
            }
            }
        }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
