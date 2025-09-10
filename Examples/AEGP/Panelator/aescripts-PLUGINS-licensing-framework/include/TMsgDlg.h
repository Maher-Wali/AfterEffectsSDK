/*
This is a class for displaying cross-platform (Mac and Windows) dialog boxes

It is rather basic in functionality, but usage is simple:
CTMsgDlg cDlg(300,400); // create dialog with size 300x400
int result = cDlg.ShowDialog(
	"titleText", //title of dialog
	"msgText", //message to be displayed
	"OK", //caption of first button
	"Cancel", //optional: caption of second button
	"",  //optional: caption of third button
	true, //true if input textbox should also be displayed
	"" // default text for input text box
	);

Return values:
result: 1 if dialog was closed by pressing one of the buttons, 0 if dialog was closed without pressing a button
cDlg.lBtnSelected contains the button pressed (1,2,3), -1 if the dialog was cancelled
cDlg.sInputText contains the text in the input text box
*/

#ifndef		TMSGDLG_H
#define		TMSGDLG_H

// #define OS_WIN

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define OS_WIN
#else
#define OS_MAC
#endif

#ifdef OS_WIN
#pragma warning (disable:4996)
#include <windows.h>
#endif
#ifdef OS_MAC
#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFDictionary.h>
#endif

class CTMsgDlg {
private:
#ifdef OS_WIN
	HWND hwndParent, hwndMsgBox, hwndMsgLabel, hwndBtn1, hwndBtn2, hwndBtn3, hwndEditBox;
	HINSTANCE hThisInstance;
	char cClassName[19];
	WORD wInputMaxLength;
#endif
#ifdef OS_MAC
#define HINSTANCE long
#define HWND long
#endif
	bool bShowInputBox;
	long lLeftOffset, lTopOffset, lWidth, lHeight, lBtnWidth, lBtnHeight; 
public:
	bool bResult;
	long lBtnSelected;
	char cTitleText[100], cMsgText[1024], cBtn1Text[100], cBtn2Text[100], cBtn3Text[100], cInputText[100];

	CTMsgDlg(long width = 500, long height = 200, long btnWidth = 80, long btnHeight = 30,
		long leftOffset = 10, long topOffset = 6, HINSTANCE hInst = 0);
	~CTMsgDlg();
#ifdef OS_WIN
	void create(HWND hwndNew);
	void show();
	void hide();
	void submit();
	void close();
	void destroy();
#endif
	int ShowDialog(
		const char* titleText, const char* msgText, 
		const char* btn1Text = "OK", const char* btn2Text = "Cancel", const char* btn3Text = "", 
		bool showInput = false, const char* inputText = "", 
		HWND hParentWindow = 0);
};

#ifdef OS_WIN

#ifndef GWL_USERDATA
#define GWL_USERDATA        (-21)
#endif

LRESULT CALLBACK TMB_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	int id = GetCurrentThreadId();
	switch (msg) {
		case WM_CREATE: {
			CTMsgDlg* _this = (CTMsgDlg*)((CREATESTRUCT*)lParam)->lpCreateParams;
			SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)_this);
			_this->create(hWnd);
			break;
			}
		case WM_COMMAND: {
				CTMsgDlg* _this = (CTMsgDlg *)GetWindowLongPtr(hWnd, GWL_USERDATA);
				switch(LOWORD(wParam)) {
					case IDOK: 
					case IDABORT: 
					case IDCANCEL: {
						_this->lBtnSelected = LOWORD(wParam);
						_this->submit();
						_this->close();
						break;
						}
				}
			}
			break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

CTMsgDlg::CTMsgDlg(long width, long height, long btnWidth, long btnHeight, long leftOffset, long topOffset, HINSTANCE hInst) {
	WNDCLASSEXA wndMsgBox;
	RECT rect;
	memset(this, 0, sizeof(wndMsgBox));
	this->hThisInstance = hInst;
	strcpy(cTitleText, "Title");
	strcpy(cMsgText, "Message");
	strcpy(cBtn1Text, "OK");
	strcpy(cBtn2Text, "");
	strcpy(cBtn3Text, "Cancel");
	bShowInputBox = true;
	lLeftOffset = leftOffset;
	lTopOffset = topOffset;
	lWidth = width;
	lHeight = height;
	lBtnWidth = btnWidth;
	lBtnHeight = btnHeight;
	lBtnSelected = -1;
	wInputMaxLength = 100;

	wndMsgBox.cbSize = sizeof(wndMsgBox);
    strcpy(cClassName, "TMsgDlg0123456789");
	for (int i = 7; i < 18; i++) cClassName[i] = (char)(65 + rand() % 26);
	wndMsgBox.lpszClassName = cClassName;
	wndMsgBox.style = CS_HREDRAW | CS_VREDRAW;
	wndMsgBox.lpfnWndProc = TMB_WndProc;
	wndMsgBox.lpszMenuName = NULL;
	wndMsgBox.hIconSm = NULL;
	wndMsgBox.cbClsExtra = 0;
	wndMsgBox.cbWndExtra = 0;
	wndMsgBox.hInstance = hInst;
	wndMsgBox.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wndMsgBox.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndMsgBox.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	RegisterClassExA(&wndMsgBox);
	if (this->hwndParent) {
		GetWindowRect(this->hwndParent, &rect);
	} else  {
		GetWindowRect(GetDesktopWindow(), &rect);
	}

	this->hwndMsgBox = CreateWindowA(cClassName, cTitleText,
		(WS_BORDER | WS_CAPTION), rect.left + (rect.right - rect.left - lWidth) / 2,
		rect.top + (rect.bottom - rect.top - lHeight) / 2,
		lWidth, lHeight, this->hwndParent, NULL,
		this->hThisInstance, this);
}

void CTMsgDlg::destroy() {
	EnableWindow(this->hwndParent, true);
	DestroyWindow(this->hwndMsgBox);
}

CTMsgDlg::~CTMsgDlg() {
	destroy();
	UnregisterClassA(cClassName, this->hThisInstance);
}

void CTMsgDlg::submit() {
	this->bResult = true;
	strcpy(cInputText, "");

	WORD wInputLength = (int)SendMessage(this->hwndEditBox, EM_LINELENGTH, 0, 0);
	if (wInputLength) {
		LPWORD lpw = (LPWORD)(&cInputText[0]);
		*lpw = wInputMaxLength;
		wInputLength = (WORD)SendMessage(this->hwndEditBox, EM_GETLINE, 0, (LPARAM)cInputText);
	}
	cInputText[wInputLength] = '\0';
	PostMessage(this->hwndMsgBox, WM_CLOSE, 0, 0);
}

void CTMsgDlg::create(HWND hwndNew) {
	HFONT myFont;
	this->hwndMsgBox = hwndNew;
	myFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

	SetWindowPos(hwndMsgBox, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	long nwid = lWidth - 2 * lLeftOffset;
	long swid = lLeftOffset + nwid / 3;
	long pwid = ((swid - lBtnWidth) / 2) - lLeftOffset;

	this->hwndMsgLabel = CreateWindowA("Static", "", WS_CHILD | WS_VISIBLE,
		lLeftOffset, lTopOffset, nwid, lBtnHeight * 4,
		this->hwndMsgBox, NULL, this->hThisInstance, NULL);
	this->hwndEditBox = CreateWindowA("Edit", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT, 
		lLeftOffset, 
		lHeight - (lTopOffset * 2) - lBtnHeight * 4, 
		nwid - 2 * lLeftOffset, lBtnHeight * 2 / 3,
		this->hwndMsgBox, NULL, this->hThisInstance, NULL);
	this->hwndBtn1 = CreateWindowA("Button", cBtn1Text, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
		lLeftOffset, lHeight - lTopOffset * 6 - lBtnHeight * 2,
		lBtnWidth, lBtnHeight, this->hwndMsgBox, (HMENU)IDOK,
		this->hThisInstance, NULL);
	this->hwndBtn2 = CreateWindowA("Button", cBtn2Text,
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_PUSHBUTTON,
		2 * lLeftOffset + swid, lHeight - lTopOffset * 6 - lBtnHeight * 2, lBtnWidth, lBtnHeight,
		this->hwndMsgBox, (HMENU)IDCANCEL, this->hThisInstance, NULL);
	this->hwndBtn3 = CreateWindowA("Button", cBtn3Text,
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
		lLeftOffset + swid * 2 + pwid, lHeight - lTopOffset * 6 - lBtnHeight * 2, lBtnWidth, lBtnHeight,
		this->hwndMsgBox, (HMENU)IDABORT, this->hThisInstance, NULL);
	SendMessageA(this->hwndMsgLabel, WM_SETFONT, (WPARAM)myFont, FALSE);
	SendMessageA(this->hwndEditBox, WM_SETFONT, (WPARAM)myFont, FALSE);
	SendMessageA(this->hwndBtn1, WM_SETFONT, (WPARAM)myFont, FALSE);
	SendMessageA(this->hwndBtn2, WM_SETFONT, (WPARAM)myFont, FALSE);
	SendMessageA(this->hwndBtn3, WM_SETFONT, (WPARAM)myFont, FALSE);
}

void CTMsgDlg::close() {
	PostMessageA(this->hwndMsgBox, WM_CLOSE, 0, 0);
}

void CTMsgDlg::hide() {
	ShowWindow(this->hwndMsgBox, SW_HIDE);
}

void CTMsgDlg::show() {
	SetWindowTextA(this->hwndMsgBox, cTitleText);
	SetWindowTextA(this->hwndEditBox, cInputText);
	SetWindowTextA(this->hwndMsgLabel, cMsgText);
	SetWindowTextA(this->hwndBtn1, cBtn1Text);
	SetWindowTextA(this->hwndBtn2, cBtn2Text);
	SetWindowTextA(this->hwndBtn3, cBtn3Text);
	SendMessageA(this->hwndEditBox, EM_LIMITTEXT, wInputMaxLength, 0);
	SendMessageA(this->hwndEditBox, EM_SETSEL, 0, -1);
	SetFocus(this->hwndEditBox);
	ShowWindow(this->hwndMsgBox, SW_NORMAL);
	if (!this->bShowInputBox) ShowWindow(this->hwndEditBox, SW_HIDE);
	if (strlen(cBtn1Text) == 0) ShowWindow(this->hwndBtn1, SW_HIDE);
	if (strlen(cBtn2Text) == 0) ShowWindow(this->hwndBtn2, SW_HIDE);
	if (strlen(cBtn3Text) == 0) ShowWindow(this->hwndBtn3, SW_HIDE);
}

int CTMsgDlg::ShowDialog(const char* titleText, const char* msgText, const char* btn1Text, const char* btn2Text, const char* btn3Text, bool showInput, const char* inputText, HWND hParentWindow) {
	MSG	msg;
	BOOL bRet;

	try {
		strcpy(cBtn1Text, btn1Text);
		strcpy(cBtn2Text, btn2Text);
		strcpy(cBtn3Text, btn3Text);
		strcpy(cInputText, inputText);
		strcpy(cTitleText, titleText);
		strcpy(cMsgText, msgText);
		bShowInputBox = showInput;
		EnableWindow(hwndParent, false);

		this->hwndParent = hParentWindow;
		this->bResult = false;
		this->show();

	} catch(...) {
		return -1;
	}

	while ((bRet = GetMessage(&msg, this->hwndMsgBox, 0, 0)) != 0) {
		try {

			if (bRet == -1) {
				int error = 1;
			} else {
				if (msg.message == WM_KEYDOWN) {
					switch (msg.wParam) {
						case VK_RETURN: { this->lBtnSelected = 3; this->submit(); } break; 
						case VK_ESCAPE: { this->lBtnSelected = -1; this->close(); } break;
						default: TranslateMessage(&msg); break;
					}
				} else {
					TranslateMessage(&msg);
				}
				DispatchMessage(&msg);	
			}
			if (msg.message == WM_CLOSE) {
				break;
			}
		} catch(...) {
			return -1;
		}
	}
	return this->bResult?1:0;
}
#endif

#ifdef OS_MAC
char* sc(CFStringRef aString);

CTMsgDlg::CTMsgDlg(long width, long height, long btnWidth, long btnHeight, long leftOffset, long topOffset, HINSTANCE hInst) {
	bShowInputBox = true;
	lLeftOffset = leftOffset;
	lTopOffset = topOffset;
	lWidth = width;
	lHeight = height;
	lBtnWidth = btnWidth;
	lBtnHeight = btnHeight;
	lBtnSelected = -1;
}

char* sc(CFStringRef aString) {
	if (aString == NULL) return NULL;
	
	CFIndex length = CFStringGetLength(aString);
	CFIndex maxSize = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8);
	char *buffer = (char *)malloc(maxSize);
	if (CFStringGetCString(aString, buffer, maxSize, kCFStringEncodingUTF8)) {
		return buffer;
	}
	return NULL;
}

int CTMsgDlg::ShowDialog(const char* titleText, const char* msgText, const char* btn1Text, const char* btn2Text, const char* btn3Text, bool showInput, const char* inputText, HWND hParentWindow) {
    strcpy(cBtn1Text, btn1Text);
    strcpy(cBtn2Text, btn2Text);
    strcpy(cBtn3Text, btn3Text);
    strcpy(cInputText, inputText);
    strcpy(cTitleText, titleText);
    strcpy(cMsgText, msgText);
	bShowInputBox = showInput;
	{
		CFUserNotificationRef dialog;
		SInt32 error;
		CFDictionaryRef dialogTemplate;
		CFOptionFlags responseFlags;
		int button;
		CFStringRef inputRef;
		const void* keysInp[] = {
			kCFUserNotificationAlertHeaderKey,
			kCFUserNotificationTextFieldTitlesKey,
			kCFUserNotificationDefaultButtonTitleKey,
			kCFUserNotificationAlternateButtonTitleKey,
			kCFUserNotificationOtherButtonTitleKey
		};
		const void* keysNoInp[] = {
			kCFUserNotificationAlertHeaderKey,
			kCFUserNotificationDefaultButtonTitleKey,
			kCFUserNotificationAlternateButtonTitleKey,
			kCFUserNotificationOtherButtonTitleKey
		};       
        char tmp[2048] = { 0 };
        strcat(tmp, titleText);
        strcat(tmp, "\r");
        strcat(tmp, msgText);
		CFStringRef c2 = CFStringCreateWithCString(kCFAllocatorDefault, tmp, kCFStringEncodingMacRoman);
        
		CFStringRef c3 = CFStringCreateWithCString(kCFAllocatorDefault, btn3Text, kCFStringEncodingMacRoman);
		CFStringRef c4 = CFStringCreateWithCString(kCFAllocatorDefault, btn1Text, kCFStringEncodingMacRoman);
		CFStringRef c5 = CFStringCreateWithCString(kCFAllocatorDefault, btn2Text, kCFStringEncodingMacRoman);
		const void* valuesInp[] = {
			c2,
			CFSTR(""),
			c3,
			c4,
			c5
		};
		const void* valuesNoInp[] = {
			c2,
			c3,
			c4,
			c5
		};
		if (showInput) {
			dialogTemplate = CFDictionaryCreate(kCFAllocatorDefault, keysInp, valuesInp,
				sizeof(keysInp)/sizeof(*keysInp),
				&kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
		} else {
			dialogTemplate = CFDictionaryCreate(kCFAllocatorDefault, keysNoInp, valuesNoInp,
												sizeof(keysNoInp)/sizeof(*keysNoInp),
												&kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
		}
		this->lBtnSelected = -1;
		dialog = CFUserNotificationCreate(
			kCFAllocatorDefault, 0,
			kCFUserNotificationNoteAlertLevel,
			&error, dialogTemplate);
//        CFRelease(c1);
		CFRelease(c2);
		CFRelease(c3);
		CFRelease(c4);
		CFRelease(c5);
		if (error) return 0;
		error = CFUserNotificationReceiveResponse(dialog, 0, &responseFlags);
		if (error) return 0;
		if (showInput) {
			inputRef = CFUserNotificationGetResponseValue(dialog, 
				kCFUserNotificationTextFieldValuesKey, 
				0);
			char* output = sc(inputRef);
			if (output) {
				strcpy(cInputText, (const char*)output);
				delete output;
			}
		}
		button = responseFlags & 0x3;
		if (button == kCFUserNotificationAlternateResponse) this->lBtnSelected = 1;
		else if (button == kCFUserNotificationOtherResponse) this->lBtnSelected = 2;
		else if (button == kCFUserNotificationDefaultResponse) this->lBtnSelected = 3;
		else return 0;
	}
	return 1;
}

CTMsgDlg::~CTMsgDlg() {
}

#endif


#endif


