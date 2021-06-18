// Editor.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "EntryPoint.h"
#include "Editor.h"
#include "External/ImGui/imgui_impl_win32.h"
#include <iostream>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
Editor::Editor* g_pEditor = nullptr;

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
    
    Editor::Editor diveEditor;
    g_pEditor = &diveEditor;

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_EDITOR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_EDITOR));

    MSG msg;
    ZeroMemory(&msg, sizeof(msg));

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
            diveEditor.Run();
        }
    }

    APP_TRACE("DiveEditor를 종료합니다.");
    g_pEditor = nullptr;

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

    wcex.style          = 0;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EDITOR));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = nullptr;
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

   // 역시 위치가 에바다.
   // 현재 Editor가 생성자에서 FilePath를 초기화 하였지만
   // Settings를 사용한다면 Editor가 초기화를 끝낸 후에나 불러 올 수 있다.
   // 가장 깔끔한 방법은 역시나 초반엔 윈도우를 숨기고,
   // Editor나 Runtime에서 크기를 확정한 후 Show하는 거다. 
   Dive::IniHelper ini(g_pEditor->GetIniFilePath());
   bool bMaximize   = ini["Window"]["bMaximize"] << false;
   int width        = ini["Window"]["Width"] << 800;
   int height       = ini["Window"]["Height"] << 600;
   int posX         = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
   int posY         = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

   if (!Dive::FileSystemHelper::FileExists(g_pEditor->GetIniFilePath()))
   {
       ini["Window"]["Width"] = width;
       ini["Window"]["Height"] = height;
       ini["Window"]["bMaximize"] = bMaximize;
   }

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      posX, posY, width, height, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   g_pEditor->SetWindow(hWnd, false);

   ShowWindow(hWnd, bMaximize ? SW_MAXIMIZE : SW_SHOWDEFAULT);
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
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;

    switch (message)
    {
    case WM_SIZE:
        {
            // 메시지로 날리면 초기화 확인 같은건 안해도 될거다.
            // 그리고 아래 에디터에서 호출하는 함수도
            // Runtime에서 이벤트로 받을 수 있다.
            EVENT_FIRE_DATA(Dive::eEventType::ChangedResolution, wParam);

          //  if (!g_pEditor->IsInitialized())   return 0;
            unsigned int width = lParam & 0xFFFF;
            unsigned int height = (lParam >> 16) & 0xFFFF;
          //  auto pGraphicsDevice = Dive::Renderer::GetInstance().GetGraphicsDevice();
          //  if (pGraphicsDevice->IsInitialized())
          //      pGraphicsDevice->ResizeBuffers(width, height);

            // ini를 갱신해야 한다.
            // 에디터가 아닌 Runtime에 구현해야한다.
            g_pEditor->ResizeWindow(width, height);
        }
        break;
    case WM_DPICHANGED:
        {
        }
        break;
    case WM_INPUT:
        {
        }
        break;
    // 아래의 포커스 부분 때문에 위젯이 영역 밖으로 나가면 제어가 불가능해진다.
    // 추후 필요가 없다고 생각되면 해당 구문을 삭제하자.
    case WM_KILLFOCUS:
       // g_editor.ActiveWindow(false);
        break;
    case WM_SETFOCUS:
       // g_editor.ActiveWindow(true);
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