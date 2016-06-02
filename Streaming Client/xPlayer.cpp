#include "xPlayer.h"

#include "logger.h"

#include "monitor.h"

#include <thread>

namespace xpNS
{
	monitor *playerClock = monitor::getInstance();
	logger *playerLogger = logger::getInstance();

	//������ģ�飺ͼ������
	HANDLE hsPlayerInput = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::playerInput);

	//������ģ�飺��ȡ���˲�����֪ͨ�м��ȡ��
	HANDLE hsPlayerOutput = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::playerOutput);

	HANDLE heCloseClient = CreateEvent(NULL, TRUE, FALSE, syncManager::ESCPressed);

	//��ʱ
	//HANDLE hsTimeOut = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::timeOut);
}
using namespace xpNS;

queue<unsigned char> xPlayer::cmdQueue;
queue<myImage> xPlayer::imgQueue;

xPlayer * xPlayer::getInstance()
{
	// xx
	// ע��ȫ�ֱ�����ʼ��˳��ԭ�������� new ������ָ�루ȫ�ֱ������������ﷵ�ص�ʱ��
	// ������Ϊ getInstance ���� new �����ã�����ȫ�ֱ����ĳ�ʼ�������� getInstance�������Ǹ�ȫ�ֱ����ĳ�ʼ��˳�����ǰ��
	// �����·��� nullptr
	// ��������Ӧ��������һʧ��д��

	static xPlayer ret;

	return &ret;
}

void xPlayer::startPlayer()
{
	//TODO�������ĳ�ʼ�����룬�����Ҫ

	playerThread = std::thread([]()
	{
		playThreadFunc(0);
	});
}

void xPlayer::insertImage(imgHead head, shared_ptr<vector<BYTE>> image)
{
	myImage img;
	img.head = head;
	img.imgData = image;

	imgQueue.push(img);

	ReleaseSemaphore(hsPlayerInput, 1, NULL);
}

bool xPlayer::getCtrlKey(unsigned char & key)
{
	if (cmdQueue.empty())
	{
		return false;
	}

	key = cmdQueue.front();

	cmdQueue.pop();

	return true;
}

HWND gHwnd = NULL;
HDC gWindowHdc = NULL;

ATOM RegisterXPlayerWindowClass()
{
	WNDCLASSEXW wcex{0};

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = [](HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)->LRESULT
	{
		switch (message)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	};
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    //wcex.hInstance      = hInstance;
    //wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DRAWONWINDOW));
    //wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    //wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_DRAWONWINDOW);
    wcex.lpszClassName  = L"XPLAYER";
    //wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL CreateXPlayerWindow()
{
	HWND hWnd = CreateWindowExW(0L, L"XPLAYER", L"XPLAYER", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, NULL, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, SW_NORMAL);
	UpdateWindow(hWnd);

	gHwnd = hWnd;
	gWindowHdc = GetDC(gHwnd);

	return TRUE;
}

#define     GetWindowStyle(hwnd)    ((DWORD)GetWindowLong(hwnd, GWL_STYLE))
#define     GetWindowExStyle(hwnd)  ((DWORD)GetWindowLong(hwnd, GWL_EXSTYLE))

void DrawOnXPlayWindow(vector<BYTE> & buffer, size_t width, size_t height)
{
	if (gWindowHdc == NULL)
		return;

	// resize window (client area) to match width and height
	//

	RECT originalWindowRect;
	RECT adjustedWindowRect;

	GetWindowRect(gHwnd, &originalWindowRect);
	GetWindowRect(gHwnd, &adjustedWindowRect);

	adjustedWindowRect.right = adjustedWindowRect.left + width;
	adjustedWindowRect.bottom = adjustedWindowRect.top + height;

	AdjustWindowRectEx(&adjustedWindowRect,                // pointer to the RECT structure to use
                   GetWindowStyle(gHwnd),     // window styles
                   FALSE,    // TRUE if the window has a menu, FALSE if not
                   GetWindowExStyle(gHwnd));  // extended window styles

	MoveWindow(gHwnd, originalWindowRect.left, originalWindowRect.top, adjustedWindowRect.right - adjustedWindowRect.left, adjustedWindowRect.bottom - adjustedWindowRect.top, true);

	RECT clientRect;
	GetClientRect(gHwnd, &clientRect);

	// "set" images in buffer to screen
	//

	BITMAPINFO bi{0};

	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); // ? size of the structure?
	bi.bmiHeader.biWidth = width;
	bi.bmiHeader.biHeight = height;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = sizeof(DWORD) * 8;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = width * height * sizeof(DWORD);
	bi.bmiHeader.biXPelsPerMeter = 72;
	bi.bmiHeader.biYPelsPerMeter = 72;
	bi.bmiHeader.biClrUsed = 0;
	bi.bmiHeader.biClrImportant = 0;

	auto error_code = SetDIBitsToDevice(gWindowHdc, clientRect.left, clientRect.top, width, height, 0, 0, 0, height, buffer.data(), &bi,  DIB_RGB_COLORS);
}

void old_DrawOnWindow()
{
	if (gWindowHdc == NULL)
		return;

	RECT clientRect;

	if(!GetClientRect(gHwnd, &clientRect))
		return;

	auto width = clientRect.right - clientRect.left;
	auto height = clientRect.bottom - clientRect.top;

	vector<DWORD> buffer;
	buffer.resize(width * height);

	for (auto x = 0; x < width; ++x)
		for (auto y = 0; y < height; ++y)
		{
			buffer[y * width + x] = rand();
		}

	BITMAPINFO bi{0};

	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); // ? size of the structure?
	bi.bmiHeader.biWidth = width;
	bi.bmiHeader.biHeight = height;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = sizeof(DWORD) * 8;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = width * height * sizeof(DWORD);
	bi.bmiHeader.biXPelsPerMeter = 72;
	bi.bmiHeader.biYPelsPerMeter = 72;
	bi.bmiHeader.biClrUsed = 0;
	bi.bmiHeader.biClrImportant = 0;

	auto error_code = SetDIBitsToDevice(gWindowHdc, clientRect.left, clientRect.top, width, height, 0, 0, 0, height, buffer.data(), &bi,  DIB_RGB_COLORS);
}


DWORD xPlayer::playThreadFunc(LPVOID lparam)
{
	// TODO �����ﴴ������
	//

	RegisterXPlayerWindowClass();

	CreateXPlayerWindow();


	imgHead imageHead;

	myImage img;

	BYTE inputKey;

	shared_ptr<vector<BYTE>> imageData;

	while (1)
	{
		// ������ peek message
		//

		MSG msg;

		size_t newKeydown = 0;

		while (PeekMessageW(&msg, nullptr, 0, 0, TRUE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);

			if (msg.message == WM_KEYDOWN)
			{
				cout << "KEYDOWN" << endl;

				cmdQueue.push(msg.wParam);

				newKeydown++;
			}
		}

		// �������ͼ����ͼ
		//

		if (WaitForSingleObject(hsPlayerInput, 0) == WAIT_OBJECT_0)
		{
			if (getImage(img))
			{
				//old_DrawOnWindow();
				DrawOnXPlayWindow(*img.imgData, img.head.xAxis.width, img.head.yAxis.height);
			}
		}

		if (newKeydown > 0)
		{
			//�����룬��֪ͨ�м����ȡ��
			

			ReleaseSemaphore(hsPlayerOutput, newKeydown, NULL);
		}
	}

	return 0;
}

bool xPlayer::getImage(myImage & img)
{
	if (imgQueue.empty())
	{
		return false;
	}

	img = imgQueue.front();

	imgQueue.pop();

	return true;
}

xPlayer::xPlayer()
{

}

xPlayer::~xPlayer()
{
	if (playerThread.joinable())
		playerThread.join();
}