#include "sNapshot.h"
#include "sNapshotwNd.h"
#include "resource.h"
#include <commctrl.h>

CsNapshotWnd sNapshot;
CfIlesys fIlesys;
char sSearchTemp[255];
int iFound;

LRESULT NotifyHandler( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK sNapshotWndPrc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_DESTROY:
		sNapshot.OnExit();
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_FILE_NEWSN:
			sNapshot.OnNew();
			break;
		case ID_FILE_EXIT:
			sNapshot.OnExit();
			break;
		case ID_ACTION_UP:
			sNapshot.OnUp();
			break;
		case ID_HELP_ABOUT:
			sNapshot.OnAbout();
			break;
		case ID_VIEW_LARGEICONS:
			sNapshot.OnSwitchView(LVS_ICON);
			break;
		case ID_VIEW_SMALLICONS:
			sNapshot.OnSwitchView(LVS_SMALLICON);
			break;
		case ID_VIEW_LISTVIEW:
			sNapshot.OnSwitchView(LVS_LIST);
			break;
		case ID_RCLK_OPEN:
			sNapshot.OpenInExplorer("open");
			break;
		case ID_RCLK_EXPLORE:
			sNapshot.OpenInExplorer("explore");
			break;
		case ID_CD_OPEN:
			sNapshot.OnLButtonDblClk();
			break;
		case ID_FILE_OPENFILE:
			sNapshot.OnLButtonDblClk();
			break;
		case ID_CD_REMOVE:
			sNapshot.OnRemove();
			break;
		case ID_CD_PROPERTIES:
			sNapshot.OnProperties();
			break;
		case ID_FILE_FILEPROP:
			sNapshot.OnFileProperties();
			break;
		case ID_FILE_OPTIONS:
			sNapshot.OnOptions();
			break;
		case ID_FILE_FIND:
			sNapshot.OnFind();
			break;
		}
		break;
	case WM_SIZE:
		DefWindowProc(hwnd,uMsg,wParam,lParam);
		sNapshot.OnSize();
		break;
	case WM_NOTIFY:
		if(((LPNMHDR) lParam)->code==NM_DBLCLK)
			sNapshot.OnLButtonDblClk();
		if(((LPNMHDR) lParam)->code==NM_RETURN)
			sNapshot.OnLButtonDblClk();
		if(((LPNMHDR) lParam)->code==NM_RCLICK)
			sNapshot.OnRClick(((LPNMITEMACTIVATE)lParam)->hdr.hwndFrom, ((LPNMITEMACTIVATE)lParam)->iItem, ((POINT)((LPNMITEMACTIVATE)lParam)->ptAction).x, ((POINT)((LPNMITEMACTIVATE)lParam)->ptAction).y);
		break;
	case WM_USER:
		sNapshot.ReInit();
		break;
	default:
		return DefWindowProc(hwnd,uMsg,wParam,lParam);
	}
	return SUCCESS;
}

BOOL CALLBACK OptionsDialogFunc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    switch (uMsg) 
    { 
        case WM_COMMAND: 
            switch (LOWORD(wParam)) 
            { 
                case IDOK: 
					{
					char sTemp1[255], sTemp2[255];
					GetDlgItemText(hwndDlg, IDC_SNAPSHOTS, sTemp1, 255);
					if(sTemp1[strlen(sTemp1)-1]=='\\')
						sTemp1[strlen(sTemp1)-1]='\0';
					if(GetFileAttributes(sTemp1)==-1)
					{
						MessageBox(hwndDlg, "Snapshots path not found", "Error", MB_OK | MB_ICONSTOP);				
						return TRUE;
					}
					strcat(sTemp1, "\\");
					GetDlgItemText(hwndDlg, IDC_TEMP, sTemp2, 255);
					if(sTemp2[strlen(sTemp2)-1]=='\\')
						sTemp2[strlen(sTemp2)-1]='\0';
					if(GetFileAttributes(sTemp2)==-1)
					{
						MessageBox(hwndDlg, "Temp path not found", "Error", MB_OK | MB_ICONSTOP);				
						return TRUE;
					}
					strcat(sTemp2, "\\");
					strcpy(sCDsPath, sTemp1);
					strcpy(sTmpPath, sTemp2);
                    EndDialog(hwndDlg, wParam); 
					EnableWindow(sNapshot.GetWindow(),TRUE);
					SendMessage(GetParent(hwndDlg), WM_USER, 0, 0);
                    return TRUE; 
					}
                case IDCANCEL: 
                    EndDialog(hwndDlg, wParam); 
					EnableWindow(sNapshot.GetWindow(),TRUE);
                    return TRUE; 
            }
			break;
    } 
    return FALSE; 
}

BOOL CALLBACK FindDialogFunc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    switch (uMsg) 
    { 
		case WM_INITDIALOG:
			{
				LV_COLUMN   lvColumn;
				HWND hwndListView=GetDlgItem(hwndDlg, IDC_RESULTS);
				lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
				lvColumn.fmt = LVCFMT_LEFT;	
				lvColumn.cx = 180;
				lvColumn.pszText ="Name"; 
				ListView_InsertColumn(hwndListView, 0, &lvColumn);
				lvColumn.cx = 100;
				lvColumn.pszText ="Container Label"; 
				ListView_InsertColumn(hwndListView, 1, &lvColumn);
			}
			break;
        case WM_COMMAND: 
            switch (LOWORD(wParam)) 
            {
				case ID_SEARCH_OPENFLDR:
					{
					HWND hwndListView=GetDlgItem(hwndDlg, IDC_RESULTS);
					int index=ListView_GetSelectionMark(hwndListView);
					if(index<0)
						return 0L;
					FILE* fp=fopen(sSearchTemp, "rb");
					int i=0;
					sEarchnode sn;
					while((fread(&sn, sizeof(sn), 1, fp)==1)&&i<index)
							i++;
					fclose(fp);
					char sPath[255];
					fIlesys.fIndPath(sn.iCD, sn.l, sPath);		
					fIlesys.oPen(sn.iCD);
					fIlesys.SetCurCD(sn.iCD);
					HWND hwndPListView=GetDlgItem(GetParent(hwndDlg), ID_LISTVIEW);
					fIlesys.sHowfolder(hwndPListView, sPath);		
					SendMessage(GetDlgItem(GetParent(hwndDlg), 1010), TB_SETSTATE, ID_ACTION_UP, MAKELONG(TBSTATE_ENABLED, 0));
					EndDialog(hwndDlg, wParam); 
					DeleteFile(sSearchTemp);
					ListView_DeleteAllItems(GetDlgItem(hwndDlg, IDC_RESULTS));
					sNapshot.UpdateCaption();
					EnableWindow(sNapshot.GetWindow(),TRUE);
					}
				break;
				case ID_SEARCH_OPEN:
					{
					HWND hwndListView=GetDlgItem(hwndDlg, IDC_RESULTS);
					int index=ListView_GetSelectionMark(hwndListView);
					if(index<0)
						return 0L;
					FILE* fp=fopen(sSearchTemp, "rb");
					int i=0;
					sEarchnode sn;
					while((fread(&sn, sizeof(sn), 1, fp)==1)&&i<index)
							i++;
					fclose(fp);
					char sPath[255];
					fIlesys.fIndPath(sn.iCD, sn.l, sPath);		
					char s[]="x:\\";
					s[0]=fIlesys.gEtcdromletter(1);
					char VolumeName[255];
					DWORD VolumeSerialNumber;
					if(GetVolumeInformation(s, VolumeName, 255, &VolumeSerialNumber, NULL, NULL, NULL, 0))
					{
					if(strcmp(VolumeName, sn.sLabel)==0)
					{
						char sTemp[255];
						strcpy(sTemp, "x:\\");
						sTemp[0]=fIlesys.gEtcdromletter(1);
						if(strlen(sPath)!=0)
						{
							strcat(sTemp, sPath);
							strcat(sTemp, "\\");
							strcat(sTemp, sn.sFileName);
						}
						else
							strcpy(sTemp, sn.sFileName);
						int err;
						if((err=(int)ShellExecute(NULL, "open",sTemp, NULL, sTmpPath, SW_SHOW))==SE_ERR_NOASSOC)
							MessageBox(hwndDlg, "No program associated with this file type.", "Error", MB_OK | MB_ICONSTOP);
						else
							if(err<32)
								MessageBox(hwndDlg, "An error ocurred while trying to complete the operation.", "Error", MB_OK | MB_ICONSTOP);
					}
					else
						MessageBox(hwndDlg, "Wrong volume in the drive.", "Error", MB_OK | MB_ICONSTOP);
					}
					else
						MessageBox(hwndDlg, "No cdrom found in the drive.", "Error", MB_OK | MB_ICONSTOP);
					}
				break;
				case IDC_FIND:
					{
						DeleteFile(sSearchTemp);
						GetDlgItemText(hwndDlg, IDC_NAME, sSearchTemp, 255);
						::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_WAIT)));
						FILE* fp=fIlesys.fInd(sSearchTemp);						
						if(fp)
						{
							HWND hwndListView=GetDlgItem(hwndDlg, IDC_RESULTS);
							ListView_DeleteAllItems(hwndListView);
							sEarchnode sn;
							int i=0;
							SendMessage(hwndListView, WM_SETREDRAW, (WPARAM)FALSE, 0);
							HIMAGELIST him=ListView_GetImageList(hwndListView, LVSIL_SMALL);
							if(!him)
							{
								him=ImageList_Create(16, 16, ILC_COLOR8|ILC_MASK  ,0, iFound+1);
								ListView_SetImageList(hwndListView, him, LVSIL_SMALL);
							}
							else
								ImageList_RemoveAll(him);
						    HICON hIcon=::LoadIcon((HINSTANCE)::GetWindowLong(::GetParent(hwndDlg), GWL_HINSTANCE), MAKEINTRESOURCE(IDI_FOLDER));
						    ImageList_AddIcon(him, hIcon);
							while(fread(&sn, sizeof(sn), 1, fp)==1)
							{
								LVITEM lvItem;
								SHFILEINFO sfi;
								lvItem.mask=LVIF_TEXT | LVIF_IMAGE;
								lvItem.iItem=i;
								lvItem.iSubItem=0;
								lvItem.cchTextMax=255;
								lvItem.pszText=LPSTR_TEXTCALLBACK;
							    ZeroMemory(&sfi,sizeof(sfi));
								/*HIMAGELIST him = (HIMAGELIST)*/SHGetFileInfo(sn.sFileName,
																	FILE_ATTRIBUTE_NORMAL,
																	&sfi,
																	sizeof(sfi),
																	SHGFI_SYSICONINDEX|SHGFI_USEFILEATTRIBUTES|SHGFI_ICON|SHGFI_SMALLICON);
//								ListView_SetImageList(hwndListView, him, LVSIL_SMALL);
								int iImage=ImageList_AddIcon(him, sfi.hIcon);
								if(sn.b)
									lvItem.iImage=0;
								else
									lvItem.iImage=iImage;
								ListView_InsertItem(hwndListView, &lvItem);
								i++;
							}
							SendMessage(hwndListView, WM_SETREDRAW, (WPARAM)TRUE, 0);
							char s[255];
							sprintf(s, "%d files found. Double click on items to open, right click for menu.", i);
							SetDlgItemText(hwndDlg, IDC_FOUND, s);
							fclose(fp);
							//sTemp now should be temp file name:
						}
						::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
					}
					break;
                case IDCANCEL: 
					{
					ListView_DeleteAllItems(GetDlgItem(hwndDlg, IDC_RESULTS));
//					ImageList_Destroy(ListView_GetImageList(GetDlgItem(hwndDlg, IDC_RESULTS), LVSIL_SMALL));
                    EndDialog(hwndDlg, wParam); 
					DeleteFile(sSearchTemp);
					EnableWindow(sNapshot.GetWindow(),TRUE);
                    return TRUE; 
					}
            }
			break;
			case WM_NOTIFY:
				return( NotifyHandler(hwndDlg, uMsg, wParam, lParam));
				break;

    } 
    return FALSE; 
}

LRESULT NotifyHandler( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LV_DISPINFO *pLvdi = (LV_DISPINFO *)lParam;
	static char szText[30];

	if (wParam != IDC_RESULTS)
		return 0L;

	switch(pLvdi->hdr.code)
	{
		case LVN_GETDISPINFO:
			{
			FILE* fp=fopen(sSearchTemp, "rb");
			int i=0;
			sEarchnode sn;
			while((fread(&sn, sizeof(sn), 1, fp)==1)&&i<pLvdi->item.iItem)
				i++;
			switch (pLvdi->item.iSubItem)
			{
				case 0:     // Name
					pLvdi->item.pszText = sn.sFileName;
					break;

				case 1:     // Container Label
					pLvdi->item.pszText = sn.sLabel;
					break;

				default:
					break;
			}
			fclose(fp);
			}
			break;
		default:
			break;
	}
	if((((LPNMHDR) lParam)->code==NM_DBLCLK)||(((LPNMHDR) lParam)->code==NM_RETURN))
	{
					HWND hwndListView=GetDlgItem(hWnd, IDC_RESULTS);
					int index=ListView_GetSelectionMark(hwndListView);
					if(index<0)
						return 0L;
					FILE* fp=fopen(sSearchTemp, "rb");
					int i=0;
					sEarchnode sn;
					while((fread(&sn, sizeof(sn), 1, fp)==1)&&i<index)
							i++;
					fclose(fp);
					char sPath[255];
					fIlesys.fIndPath(sn.iCD, sn.l, sPath);		
					char s[]="x:\\";
					s[0]=fIlesys.gEtcdromletter(1);
					char VolumeName[255];
					DWORD VolumeSerialNumber;
					if(GetVolumeInformation(s, VolumeName, 255, &VolumeSerialNumber, NULL, NULL, NULL, 0))
					{
					if(strcmp(VolumeName, sn.sLabel)==0)
					{
						char sTemp[255];
						strcpy(sTemp, "x:\\");
						sTemp[0]=fIlesys.gEtcdromletter(1);
						if(strlen(sPath)!=0)
						{
							strcat(sTemp, sPath);
							strcat(sTemp, "\\");
							strcat(sTemp, sn.sFileName);
						}
						else
							strcpy(sTemp, sn.sFileName);
						int err;
						if((err=(int)ShellExecute(NULL, "open",sTemp, NULL, sTmpPath, SW_SHOW))==SE_ERR_NOASSOC)
							MessageBox(hWnd, "No program associated with this file type.", "Error", MB_OK | MB_ICONSTOP);
						else
							if(err<32)
								MessageBox(hWnd, "An error ocurred while trying to complete the operation.", "Error", MB_OK | MB_ICONSTOP);
					}
					else
						MessageBox(hWnd, "Wrong volume in the drive.", "Error", MB_OK | MB_ICONSTOP);
					}
					else
						MessageBox(hWnd, "No cdrom found in the drive.", "Error", MB_OK | MB_ICONSTOP);
	}

	if(((LPNMHDR) lParam)->code==NM_RCLICK)
	{
		HWND hwndListView=GetDlgItem(hWnd, IDC_RESULTS);
		int index=ListView_GetSelectionMark(hwndListView);
		if(index<0)
			return 0L;
		RECT rc;
		GetWindowRect(hwndListView, &rc);
		HMENU hMenu=LoadMenu((HINSTANCE)GetWindowLong(sNapshot.GetWindow(), GWL_HINSTANCE), MAKEINTRESOURCE(IDR_RCLK));
		int x=((POINT)((LPNMITEMACTIVATE)lParam)->ptAction).x;
		int y=((POINT)((LPNMITEMACTIVATE)lParam)->ptAction).y;
		TrackPopupMenu( GetSubMenu(hMenu,4)
							,TPM_LEFTALIGN | TPM_RIGHTBUTTON
							,rc.left+x ,y+rc.top, 0, hWnd,
							NULL
							);
	}
	return 0L;
}


BOOL CALLBACK AboutDialogFunc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    switch (uMsg) 
    { 
        case WM_COMMAND: 
            switch (LOWORD(wParam)) 
            { 
                case IDOK: 
                    EndDialog(hwndDlg, wParam); 
					EnableWindow(sNapshot.GetWindow(),TRUE);
                    return TRUE; 
            }
			break;
    } 
    return FALSE; 
}

#include <malloc.h>

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd)
{
	MSG msg;
	InitCommonControls();
	sNapshot.Initialize(hInstance);
	if(sNapshot.InitializeClass(hPrevInstance)) return 0;
	sNapshot.InitializeWindow();
	while(GetMessage(&msg,NULL,0,0))
	{
		if(!sNapshot.IsDialogMessage(&msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.wParam;
}