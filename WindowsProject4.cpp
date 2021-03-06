//WindowsProject4.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "WindowsProject4.h"

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<sys/timeb.h>
#include<Windows.h>
#define N 50//city维数
#define M 100//行走最大次数
#define T 200//尝试次数

#define MAX_LOADSTRING 100


//
int randomDirection(void);//随机选择方向
long getsrand(void);//在微秒级初始化随机种子
bool selfav(void);//一次随机自行走

int trytimes = 0;//尝试次数
int deadEnds = 0;//死胡同次数
int city[N][N];//城市数组模型，0=此节点未走过，1=已经走过

typedef struct //行走轨迹记录
{
	int x;//x方向行走量
	int y;//y方向行走量
}location;
location roadLine[M];

int direction;//行走方向,0=北，1=东，2=南，3=西
int lastDirection;//上一次行走的方向
int x, y;//当前位置坐标


// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明:
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

    // TODO: 在此放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT4, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT4));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT4));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT4);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, 500, 500, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
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
			MoveToEx(hdc, 250, 250, NULL);

			x = N / 2;//初始坐标为city中心
			y = N / 2;
			city[x][y] = 1;
		//	printf("初始坐标 x=%d,y=%d\n", x, y);
		//	printf("行走方向,0=北，1=东，2=南，3=西\n");

			//初始化城市数组模型
			for (int i = 0; i < N; i++)
			{
				for (int j = 0; j < N; j++)
				{
					city[i][j] = 0;
				}
			}

			roadLine[trytimes].x = x;

			roadLine[trytimes].y = y;
			//lastDirection = 5;

			for (int i = 0; i < T; i++)
			{
				printf("第%d次尝试\n", i + 1);
				if ((x < 0 || x >= N) || (y < 0 || y >= N))//判断是否走出city
				{
					//printf("get out\n");
					return true ;
				}
				else if (city[x - 1][y] && city[x][y + 1] && city[x + 1][y] && city[x][y - 1])//判断是否走入死胡同
				{
			/*	//	printf("got in dead_end!\n\n");
				//	return false;
					
				//	允许回头版
				//	printf("当前坐标x=%d,y=%d\n", x, y);
				//	printf("got in dead-end!\n\n");

				//	printf("回退一步\n");
					x = roadLine[trytimes - 1].x;
					y = roadLine[trytimes - 1].y;
				//	printf("当前坐标x=%d,y=%d\n", x, y);
					deadEnds++;
					trytimes--;
					//printf("第%d步\n", trytimes);
			*/		
					return false;
				}
				else
				{
					//选择方向前进一步
					do
					{
						x = roadLine[trytimes].x;
						y = roadLine[trytimes].y;

						direction = randomDirection();
						switch (direction)
						{
						case 0:
							y++;
							//printf("向北前进一步.\n");
							break;
						case 1:
							x++;
							//printf("向东前进一步.\n");
							break;
						case 2:
							y--;
							//printf("向南前进一步.\n");
							break;
						case 3:
							x--;
							//printf("向西前进一步.\n");
							break;
						default:;
						}
					} while (city[x][y] == 1);

					trytimes++;
					roadLine[trytimes].x = x;
					roadLine[trytimes].y = y;
					city[x][y] = 1;
					LineTo(hdc, x * 10, y * 10);
					//	printf("第%d步\n", trytimes);
				//	printf("方向%d\n", direction);
				//	printf("当前坐标x=%d,y=%d\n", x, y);
				}

			}


        
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
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

// “关于”框的消息处理程序。
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


bool selfav(void)
{
	//初始化参数
	x = N / 2;//初始坐标为city中心
	y = N / 2;
	city[x][y] = 1;
	printf("初始坐标 x=%d,y=%d\n", x, y);
	printf("行走方向,0=北，1=东，2=南，3=西\n");

	//初始化城市数组模型
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			city[i][j] = 0;
		}
	}

	roadLine[trytimes].x = x;

	roadLine[trytimes].y = y;
	//lastDirection = 5;

	for (int i = 0; i < T; i++)
	{
		printf("第%d次尝试\n", i + 1);
		if ((x < 0 || x >= N) || (y < 0 || y >= N))//判断是否走出city
		{
			//printf("get out\n");
			return true;
		}
		else if (city[x - 1][y] && city[x][y + 1] && city[x + 1][y] && city[x][y - 1])//判断是否走入死胡同
		{
			printf("got in dead_end!\n\n");
			return false;
			/*
			允许回头版
			printf("当前坐标x=%d,y=%d\n", x, y);
			printf("got in dead-end!\n\n");

			printf("回退一步\n");
			x = roadLine[trytimes - 1].x;
			y = roadLine[trytimes - 1].y;
			printf("当前坐标x=%d,y=%d\n", x, y);
			deadEnds++;
			trytimes--;
			//printf("第%d步\n", trytimes);
			*/
		}
		else
		{
			//选择方向前进一步
			do
			{
				x = roadLine[trytimes].x;
				y = roadLine[trytimes].y;

				direction = randomDirection();
				switch (direction)
				{
				case 0:
					y++;
					//printf("向北前进一步.\n");
					break;
				case 1:
					x++;
					//printf("向东前进一步.\n");
					break;
				case 2:
					y--;
					//printf("向南前进一步.\n");
					break;
				case 3:
					x--;
					//printf("向西前进一步.\n");
					break;
				default:;
				}
			} while (city[x][y] == 1);

			trytimes++;
			roadLine[trytimes].x = x;
			roadLine[trytimes].y = y;
			city[x][y] = 1;

			//	printf("第%d步\n", trytimes);
			printf("方向%d\n", direction);
			printf("当前坐标x=%d,y=%d\n", x, y);
		}

	}

	return false;

}
long getsrand(void)
{

	LARGE_INTEGER nFreq;
	LARGE_INTEGER nBeginTime;
	LARGE_INTEGER nEndTime;
	double time;

	QueryPerformanceFrequency(&nFreq);
	QueryPerformanceCounter(&nBeginTime);
	Sleep(50);
	QueryPerformanceCounter(&nEndTime);

	time = (double)(nEndTime.QuadPart - nBeginTime.QuadPart) / (double)nFreq.QuadPart;
	//	printf("%ld\n", (long)(time * 10000000));
	return (long)(time * 10000000);
}
int   randomDirection(void)
{
	//getchar();
	srand(getsrand());
	int randInteger = rand() % 4;
	//printf("%d\t",randInteger);
	return randInteger;
}