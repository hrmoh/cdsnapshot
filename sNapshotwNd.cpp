#include "sNapshotwNd.h"
#include "commctrl.h"
#include "resource.h"
#include "fIlesys.h"

extern LRESULT CALLBACK sNapshotWndPrc(HWND hwnd,UINT Msg,WPARAM wParam,LPARAM lParam);
extern BOOL CALLBACK AboutDialogFunc(HWND hwnd,UINT Msg,WPARAM wParam,LPARAM lParam);
extern BOOL CALLBACK OptionsDialogFunc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
extern BOOL CALLBACK FindDialogFunc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
extern CfIlesys fIlesys;

char sCDsPath[255];
char sTmpPath[255];

void CsNapshotWnd::Initialize(HINSTANCE hInstance)
{
	m_hInstance=hInstance;
	fIlesys.Init();
}

int CsNapshotWnd::InitializeClass(HINSTANCE hPrevInstance)
{
	if(!m_hInstance) return NO_INITIALIZATION;
	if(hPrevInstance) return SUCCESS;
	WNDCLASS wc;
	memset(&wc,0,sizeof(wc));
	wc.hInstance=m_hInstance;
	wc.hbrBackground=(HBRUSH)COLOR_APPWORKSPACE;
	wc.lpfnWndProc=sNapshotWndPrc;
	wc.lpszClassName="sNapshotClass";
	wc.hIcon=LoadIcon(m_hInstance,MAKEINTRESOURCE(IDI_CD));	
	wc.style=CS_HREDRAW|CS_VREDRAW;
	return !RegisterClass(&wc);	
}

int CsNapshotWnd::InitializeWindow()
{
	InitializeMenu();
	DWORD iXpos, iYpos, iWidth, iHeight;
	HKEY hKey;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Snapshot", 0, KEY_ALL_ACCESS ,&hKey)==ERROR_SUCCESS)
	{
		DWORD dwSize;
		if(RegQueryValueEx(hKey, "h", 0, NULL, (BYTE*)&iHeight, &dwSize)!=ERROR_SUCCESS)
			iHeight=350;
		if(RegQueryValueEx(hKey, "w", 0, NULL, (BYTE*)&iWidth, &dwSize)!=ERROR_SUCCESS)
			iWidth=500;
		if(RegQueryValueEx(hKey, "x", 0, NULL, (BYTE*)&iXpos, &dwSize)!=ERROR_SUCCESS)
			iXpos=10;
		if(RegQueryValueEx(hKey, "y", 0, NULL, (BYTE*)&iYpos, &dwSize)!=ERROR_SUCCESS)
			iYpos=10;
	}
	else
	{
		SECURITY_ATTRIBUTES sa;
		sa.bInheritHandle=TRUE;
		sa.lpSecurityDescriptor=NULL;
		sa.nLength=sizeof(sa);
		iXpos=10;
		iYpos=10;
		iHeight=350;
		iWidth=500;
		DWORD dw=REG_CREATED_NEW_KEY;
		if(RegCreateKeyEx(HKEY_LOCAL_MACHINE, "Software\\Snapshot", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, &sa, &hKey,&dw)==ERROR_SUCCESS)
		{
			RegSetValueEx(hKey, "x", 0, REG_DWORD, (BYTE*)&iXpos, sizeof(DWORD));
			RegSetValueEx(hKey, "y", 0, REG_DWORD, (BYTE*)&iYpos, sizeof(DWORD));
			RegSetValueEx(hKey, "w", 0, REG_DWORD, (BYTE*)&iWidth, sizeof(DWORD));
			RegSetValueEx(hKey, "h", 0, REG_DWORD, (BYTE*)&iHeight, sizeof(DWORD));
		}
	}
	RegCloseKey(hKey);
	m_hwnd=CreateWindow("sNapshotClass","CD Snapshot",WS_OVERLAPPEDWINDOW,iXpos,iYpos,
		iWidth,iHeight,NULL,m_hMenu,m_hInstance,NULL);
	if(!m_hwnd) return NO_SUCCESS;
	CreateToolbar();
	CreateListView();
	ShowWindow(m_hwnd,SW_SHOW);
	UpdateWindow(m_hwnd);
	return SUCCESS;
}

void CsNapshotWnd::InitializeMenu()
{
	m_hMenu=LoadMenu(m_hInstance,MAKEINTRESOURCE(IDR_sNapshot));
}

void CsNapshotWnd::OnExit()
{
	HKEY hKey;
	RECT rc;
	DWORD iXpos, iYpos, iWidth, iHeight;
	GetWindowRect(m_hwnd, &rc);
	iXpos=rc.left;
	iYpos=rc.top;
	iHeight=rc.bottom-rc.top;
	iWidth=rc.right-rc.left;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Snapshot", 0, KEY_ALL_ACCESS ,&hKey)==ERROR_SUCCESS)
	{
		RegSetValueEx(hKey, "x", 0, REG_DWORD, (BYTE*)&iXpos, sizeof(DWORD));
		RegSetValueEx(hKey, "y", 0, REG_DWORD, (BYTE*)&iYpos, sizeof(DWORD));
		RegSetValueEx(hKey, "w", 0, REG_DWORD, (BYTE*)&iWidth, sizeof(DWORD));
		RegSetValueEx(hKey, "h", 0, REG_DWORD, (BYTE*)&iHeight, sizeof(DWORD));
		RegSetValueEx(hKey, "CDS-PATH", 0, REG_SZ, (BYTE*)sCDsPath, (strlen(sCDsPath)+1)*sizeof(char));
		RegSetValueEx(hKey, "CDS-TEMP", 0, REG_SZ, (BYTE*)sTmpPath, (strlen(sTmpPath)+1)*sizeof(char));
	}
	RegCloseKey(hKey);
	PostQuitMessage(0);
}

void CsNapshotWnd::OnAbout()
{
	m_hAboutDlg=CreateDialog(m_hInstance,MAKEINTRESOURCE(IDD_ABOUT),m_hwnd,(DLGPROC)AboutDialogFunc);	
	RECT rc1, rc2;
	GetWindowRect(m_hwnd, &rc1);
	GetWindowRect(m_hAboutDlg, &rc2);
	MoveWindow
		(m_hAboutDlg,
		rc1.left+(rc1.right-rc1.left-rc2.right+rc2.left)/2,
		rc1.top+(rc1.bottom-rc1.top-rc2.bottom+rc2.top)/2,
		rc2.right-rc2.left,
		rc2.bottom-rc2.top,
		TRUE
		);
	ShowWindow(m_hAboutDlg,SW_SHOW);
	UpdateWindow(m_hAboutDlg);	
	EnableWindow(m_hwnd,FALSE);
}

void CsNapshotWnd::OnOptions()
{
	m_hOptionsDlg=CreateDialog(m_hInstance,MAKEINTRESOURCE(IDD_OPTIONS),m_hwnd,(DLGPROC)OptionsDialogFunc);	
	SetDlgItemText(m_hOptionsDlg, IDC_SNAPSHOTS, sCDsPath);
	SetDlgItemText(m_hOptionsDlg, IDC_TEMP, sTmpPath);
	RECT rc1, rc2;
	GetWindowRect(m_hwnd, &rc1);
	GetWindowRect(m_hOptionsDlg, &rc2);
	MoveWindow
		(m_hOptionsDlg,
		rc1.left+(rc1.right-rc1.left-rc2.right+rc2.left)/2,
		rc1.top+(rc1.bottom-rc1.top-rc2.bottom+rc2.top)/2,
		rc2.right-rc2.left,
		rc2.bottom-rc2.top,
		TRUE
		);
	ShowWindow(m_hOptionsDlg,SW_SHOW);
	UpdateWindow(m_hOptionsDlg);	
	EnableWindow(m_hwnd,FALSE);
}

void CsNapshotWnd::OnSize()
{
	RECT  rc;          
	int ih=0;
	GetClientRect(m_hwnd, &rc);
	if(m_hwndToolbar)
	{
		RECT  rc2;          
		GetWindowRect(m_hwndToolbar, &rc2);
		ih=rc2.bottom-rc2.top;
		MoveWindow(m_hwndToolbar,rc.left,rc.top,rc.right - rc.left,ih,TRUE);
		ih-=4;
	}
	if(m_hwndListView)
	{
		MoveWindow(m_hwndListView,rc.left,rc.top + ih,rc.right - rc.left,rc.bottom - rc.top - ih,TRUE);
		ListView_Arrange(m_hwndListView, LVA_DEFAULT);
	}
}

void CsNapshotWnd::OnUp()
{
	if(fIlesys.bAck(m_hwndListView, ::LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_CD)))==0)
		SendMessage(m_hwndToolbar, TB_SETSTATE, ID_ACTION_UP, MAKELONG(0, 0));
	UpdateCaption();
}

void CsNapshotWnd::OnNew()
{
	char s[]="c:\\";
	s[0]=fIlesys.gEtcdromletter(1);
	if(s[0]==0)
	{
		MessageBox(m_hwnd, "No CDROM detected!", "Error", MB_OK|MB_ICONSTOP);
		return;
	}
	UINT uMode=SetErrorMode(SEM_FAILCRITICALERRORS);
	if(!GetVolumeInformation(s,NULL,0,NULL,NULL,NULL,NULL,0))
	{
		MessageBox(m_hwnd, "No disk found in the CDROM drive", "Error", MB_OK|MB_ICONSTOP);
		SetErrorMode(uMode);
		return;
	}
	SetErrorMode(uMode);
	::SetCursor(::LoadCursor(NULL, IDC_WAIT));
	fIlesys.bUildcdtRee();
	::SetCursor(::LoadCursor(NULL, IDC_ARROW));
//	fIlesys.sHowfolder(m_hwndListView, "");
	fIlesys.fIndCDContents(m_hwndListView, ::LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_CD)));
	UpdateCaption();
}

void CsNapshotWnd::CreateListView()
{
	m_hwndListView = CreateWindowEx(WS_EX_CLIENTEDGE,WC_LISTVIEW,"",
				 WS_TABSTOP|WS_CHILD|WS_BORDER|WS_VISIBLE|LVS_ICON|LVS_SINGLESEL/*|LVS_OWNERDATA*/,
                 0,0,0,0,m_hwnd,(HMENU)ID_LISTVIEW,m_hInstance,NULL);
	OnSize();
	fIlesys.fIndCDContents(m_hwndListView, ::LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_CD)));
}

void CsNapshotWnd::OnLButtonDblClk()
{
	INT iSel=ListView_GetSelectionMark(m_hwndListView);
	if(iSel!=-1)
	{
		if(fIlesys.oPen(iSel, m_hwndListView)==0)
			SendMessage(m_hwndToolbar, TB_SETSTATE, ID_ACTION_UP, MAKELONG(TBSTATE_ENABLED, 0));
		UpdateCaption();
	}
}

void CsNapshotWnd::OnSwitchView(DWORD dwView)
{
    DWORD dwStyle = GetWindowLong(m_hwndListView, GWL_STYLE);
	if((dwStyle | LVS_ICON) == dwStyle)
	{
		SendMessage(m_hwndToolbar, TB_SETSTATE, ID_VIEW_LARGEICONS, MAKELONG(TBSTATE_ENABLED, 0));
	}
	if((dwStyle | LVS_SMALLICON) == dwStyle)
	{
		SendMessage(m_hwndToolbar, TB_SETSTATE, ID_VIEW_SMALLICONS, MAKELONG(TBSTATE_ENABLED, 0));
	}
	if((dwStyle | LVS_LIST) == dwStyle)
	{
		SendMessage(m_hwndToolbar, TB_SETSTATE, ID_VIEW_LISTVIEW, MAKELONG(TBSTATE_ENABLED, 0));
	}
    if ((dwStyle & LVS_TYPEMASK) != dwView)
        SetWindowLong(m_hwndListView, GWL_STYLE, (dwStyle & ~LVS_TYPEMASK) | dwView);
	if(dwView==LVS_SMALLICON)
	{
		SendMessage(m_hwndToolbar, TB_SETSTATE, ID_VIEW_SMALLICONS, MAKELONG(TBSTATE_CHECKED | TBSTATE_ENABLED, 0));
	}
	if(dwView==LVS_ICON)
	{
		SendMessage(m_hwndToolbar, TB_SETSTATE, ID_VIEW_LARGEICONS, MAKELONG(TBSTATE_CHECKED | TBSTATE_ENABLED, 0));
	}
	if(dwView==LVS_LIST)
	{
		SendMessage(m_hwndToolbar, TB_SETSTATE, ID_VIEW_LISTVIEW, MAKELONG(TBSTATE_CHECKED | TBSTATE_ENABLED, 0));
	}
	OnSize();
}

void CsNapshotWnd::CreateToolbar()
{
	TBBUTTON tbButtons [] = 
		{
		//TBSTATE_ENABLED
			{0, ID_ACTION_UP, 0 , TBSTYLE_BUTTON, 0L, 0},
			{0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0L, 0},
			{1, ID_VIEW_LARGEICONS, TBSTATE_ENABLED | TBSTATE_CHECKED, TBSTYLE_BUTTON, 0L, 0},
			{2, ID_VIEW_SMALLICONS, TBSTATE_ENABLED , TBSTYLE_BUTTON, 0L, 0},
			{3, ID_VIEW_LISTVIEW, TBSTATE_ENABLED , TBSTYLE_BUTTON, 0L, 0},
		}; 


	m_hwndToolbar=CreateToolbarEx(m_hwnd,
								WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_TRANSPARENT, 
								1010,
								5,
								m_hInstance,
								IDB_TOOLBAR,
								tbButtons,
								5,
								16,
								16,
								16,
								16,
								sizeof(TBBUTTON)
								);
}

void CsNapshotWnd::OnRClick(HWND hwndFrom, int iItem ,int x, int y)
{
	if(hwndFrom!=m_hwndListView)
		return;
	RECT rc;
	GetWindowRect(m_hwndListView, &rc);
	HMENU hMenu=LoadMenu(m_hInstance, MAKEINTRESOURCE(IDR_RCLK));
	if(fIlesys.GetCurCD()!=-1)
	{
		if(iItem==-1)
		{
			TrackPopupMenu( GetSubMenu(hMenu,0)
							,TPM_LEFTALIGN | TPM_RIGHTBUTTON
							,rc.left+x ,y+rc.top, 0, m_hwnd,
							NULL
							);
		}
		else
		{
			TrackPopupMenu( GetSubMenu(hMenu,3)
							,TPM_LEFTALIGN | TPM_RIGHTBUTTON
							,rc.left+x ,y+rc.top, 0, m_hwnd,
							NULL
							);
		}
	}
	else
	{
		if(iItem==-1)
		{
			TrackPopupMenu( GetSubMenu(hMenu,1)
							,TPM_LEFTALIGN | TPM_RIGHTBUTTON
							,rc.left+x ,y+rc.top, 0, m_hwnd,
							NULL
							);
		}
		else
		{
			TrackPopupMenu( GetSubMenu(hMenu,2)
							,TPM_LEFTALIGN | TPM_RIGHTBUTTON
							,rc.left+x ,y+rc.top, 0, m_hwnd,
							NULL
							);
		}
	}
}

void CsNapshotWnd::OpenInExplorer(char *sAction)
{
	char sVolumeName[255];
	if(fIlesys.cHeckcurCD(sVolumeName))
	{
		char sPath[255];
		fIlesys.gEtCurPath(sPath);
		ShellExecute(NULL, sAction, sPath, NULL, "", SW_SHOW);
	}
	else
	{
		char sMsg[255];
		strcpy(sMsg, "Wrong volume in the drive!\nPlease insert cdrom labeled ");
		strcat(sMsg, sVolumeName);
		strcat(sMsg, " in the drive and then try again.");
		MessageBox(m_hwnd, sMsg, "Error", MB_OK | MB_ICONSTOP);
	}
}

void CsNapshotWnd::OnRemove()
{
	if(
		MessageBox(m_hwnd, "Delete selected item?", "Confirmation", MB_YESNO | MB_ICONQUESTION)
		==
		IDNO
	  )
	  return;
	INT iSel=ListView_GetSelectionMark(m_hwndListView);
	if(fIlesys.Remove(iSel)==FALSE)
		MessageBox(m_hwnd, "An error occured while trying to complete the operation", "Error", MB_OK | MB_ICONSTOP);
	else
		fIlesys.fIndCDContents(m_hwndListView, ::LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_CD)));
}

void CsNapshotWnd::OnProperties()
{
	INT iSel=ListView_GetSelectionMark(m_hwndListView);
	fIlesys.Properties(iSel, m_hwnd);
}

void CsNapshotWnd::OnFileProperties()
{
	INT iSel=ListView_GetSelectionMark(m_hwndListView);
	::SetCursor(::LoadCursor(NULL, IDC_WAIT));
	fIlesys.FileProperties(iSel, m_hwnd);
	::SetCursor(::LoadCursor(NULL, IDC_ARROW));
}

BOOL CsNapshotWnd::IsDialogMessage(LPMSG lpMsg)
{
	if(m_hAboutDlg)
		if(::IsDialogMessage(m_hAboutDlg, lpMsg))
			return TRUE;
	if(m_hOptionsDlg)
		if(::IsDialogMessage(m_hOptionsDlg, lpMsg))
			return TRUE;
	if(m_hFindDlg)
		if(::IsDialogMessage(m_hFindDlg, lpMsg))
			return TRUE;
	return FALSE;
}

void CsNapshotWnd::ReInit()
{
	fIlesys.fIndCDContents(m_hwndListView, ::LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_CD)));
}

void CsNapshotWnd::OnFind()
{
	m_hFindDlg=CreateDialog(m_hInstance,MAKEINTRESOURCE(IDD_FIND),m_hwnd,(DLGPROC)FindDialogFunc);	
	RECT rc1, rc2;
	GetWindowRect(m_hwnd, &rc1);
	GetWindowRect(m_hFindDlg, &rc2);
	MoveWindow
		(m_hFindDlg,
		rc1.left+(rc1.right-rc1.left-rc2.right+rc2.left)/2,
		rc1.top+(rc1.bottom-rc1.top-rc2.bottom+rc2.top)/2,
		rc2.right-rc2.left,
		rc2.bottom-rc2.top,
		TRUE
		);
	ShowWindow(m_hFindDlg,SW_SHOW);
	UpdateWindow(m_hFindDlg);	
	EnableWindow(m_hwnd,FALSE);
}

void CsNapshotWnd::UpdateCaption()
{
	if(fIlesys.GetCurCD()==-1)
	{
		SetWindowText(m_hwnd, "CD Snapshot");
		return;
	}
	DWORD dw;
	char sTitle[255];
	fIlesys.GetDCInformation(sTitle, dw);
	strcat(sTitle, " - ");
	char sPath[255];
	fIlesys.gEtCurPath(sPath);
	strcat(sTitle, sPath);
	strcpy(sPath, "CD Snapshot - ");
	strcat(sPath, sTitle);
	SetWindowText(m_hwnd, sPath);
}