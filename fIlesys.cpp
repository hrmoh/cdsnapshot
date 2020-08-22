#include "fIlesys.h"
#include <stdio.h>
#include <commctrl.h>

extern int iFound;

void SysTimeToStr(LPSYSTEMTIME lpSystemTime, char* s);

void CfIlesys::Init()
{
	HKEY hKey;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Snapshot", 0, KEY_ALL_ACCESS ,&hKey)==ERROR_SUCCESS)
	{
		DWORD dwSize;
		if(RegQueryValueEx(hKey, "CDS-PATH", 0, NULL, (BYTE*)sCDsPath, &dwSize)!=ERROR_SUCCESS)
			strcpy(sCDsPath, "");
		else
			if(sCDsPath[strlen(sCDsPath)-1]!='\\')
				strcat(sCDsPath, "\\");
		if(RegQueryValueEx(hKey, "CDS-TEMP", 0, NULL, (BYTE*)sTmpPath, &dwSize)!=ERROR_SUCCESS)
		{
			GetTempPath(255, sTmpPath);
		}
	}
	else
	{
		SECURITY_ATTRIBUTES sa;
		sa.bInheritHandle=TRUE;
		sa.lpSecurityDescriptor=NULL;
		sa.nLength=sizeof(sa);
		strcpy(sCDsPath, "");
		GetTempPath(255, sTmpPath);
		DWORD dw=REG_CREATED_NEW_KEY;
		if(RegCreateKeyEx(HKEY_LOCAL_MACHINE, "Software\\Snapshot", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, &sa, &hKey,&dw)==ERROR_SUCCESS)
		{
			RegSetValueEx(hKey, "CDS-PATH", 0, REG_SZ, (BYTE*)sCDsPath, (strlen(sCDsPath)+1)*sizeof(char));
			RegSetValueEx(hKey, "CDS-TEMP", 0, REG_SZ, (BYTE*)sTmpPath, (strlen(sTmpPath)+1)*sizeof(char));
		}
	}
	RegCloseKey(hKey);
}

char CfIlesys::gEtcdromletter(int index)
{
	char _drives[150];
	char _next[4];
	int number=0;
	GetLogicalDriveStrings(150, _drives);
	int i=0;
	do
	{
		int j=i;
		do
		{
			_next[i-j]=_drives[i];
			i++;
		}while(_drives[i]!='\0');
		_next[3]='\0';
		if(GetDriveType(_next)==DRIVE_CDROM)
			number++;
		if(number==index)
			return _next[0];
		i++;
	}while(_drives[i]!='\0');
	return 0;
}

BOOL CfIlesys::bUildcdtRee()
{
	char s[]="x:\\";
	iTreeSize=0;
	if((s[0]=gEtcdromletter(1))==0)
		return FALSE;
//
	char VolumeName[255];

	DWORD VolumeSerialNumber;
	GetVolumeInformation(s, VolumeName, 255, &VolumeSerialNumber, NULL, NULL, NULL, 0);
//

	FILE* fp;
	char sTemph[255];
	strcpy(sTemph, sTmpPath);
	strcat(sTemph, "cd.tmp");
	fp=fopen(sTemph, "wb");
	bUildtRee(-1, s, iTreeSize, fp);
	fclose(fp);

	int i=0;
	int k=0;
	long l=0;
	long l1=0;
	int lfthr=-1;
	fIlenode f;
	strcpy(sTemph, sTmpPath);
	strcat(sTemph, "cd");
	FILE* fp1=fopen(sTemph, "wb");

//write dir. tree:
	strcpy(sTemph, sTmpPath);
	strcat(sTemph, "cd.tmp");
	fp=fopen(sTemph, "rb");
	do
	{
	do
	{
		fread(&f, sizeof(f), 1, fp);
		i++;
	}while(f.fthr==lfthr&&i<iTreeSize);
	if(f.fthr!=lfthr)
	{
		lfthr=f.fthr;
		l1=ftell(fp);
		fclose(fp);		
		if(lfthr<k)
		{
			strcpy(sTemph, sTmpPath);
			strcat(sTemph, "cd");
			fp=fopen(sTemph, "rb");
			long lll=ftell(fp);
			strcpy(sTemph, sTmpPath);
			strcat(sTemph, "cd.tmp1");
			FILE* fp2=fopen(sTemph, "wb");
			for(int j=0; j<lfthr; j++)
			{
				if(fread(&f, sizeof(f), 1, fp)!=1)
					break;
				fwrite(&f, sizeof(f), 1, fp2);
			}
			fread(&f, sizeof(f), 1, fp);
			f.chld1st=i;
			fwrite(&f, sizeof(f), 1, fp2);
			for(int j=lfthr+1; j<k; j++)
			{
				fread(&f, sizeof(f), 1, fp);
				fwrite(&f, sizeof(f), 1, fp2);
			}
			fclose(fp);
			fclose(fp2);
			strcpy(sTemph, sTmpPath);
			strcat(sTemph, "cd");
			fp=fopen(sTemph, "wb");
			strcpy(sTemph, sTmpPath);
			strcat(sTemph, "cd.tmp1");
			fp2=fopen(sTemph, "rb");
			for(int j=0; j<k; j++)
			{
				fread(&f, sizeof(f), 1, fp2);
				fwrite(&f, sizeof(f), 1, fp);
			}
			fclose(fp);
			fclose(fp2);	
			strcpy(sTemph, sTmpPath);
			strcat(sTemph, "cd.tmp");
			fp=fopen(sTemph, "rb");
		}
		else
		{
		strcpy(sTemph, sTmpPath);
		strcat(sTemph, "cd.tmp");
		fp=fopen(sTemph, "rb");
		fseek(fp, l, 0);
		for(int j=k; j<lfthr; j++)
		{
			fread(&f, sizeof(f), 1, fp);
			fwrite(&f, sizeof(f), 1, fp1);
		}
		fread(&f, sizeof(f), 1, fp);
		l=ftell(fp);
		k=lfthr+1;
		f.chld1st=i;
		fwrite(&f, sizeof(f), 1, fp1);
		fflush(fp1);
		}
		fseek(fp, l1, 0);
	}
	}
	while(i<iTreeSize);
	fseek(fp, l, 0);
	for(int j=k; j<iTreeSize; j++)
	{
		fread(&f, sizeof(f), 1, fp);
		fwrite(&f, sizeof(f), 1, fp1);
	}
	fclose(fp);
	fclose(fp1);

	strcpy(sTemph, sTmpPath);
	strcat(sTemph, "cd.tmp");
	DeleteFile(sTemph);
	strcpy(sTemph, sTmpPath);
	strcat(sTemph, "cd.tmp1");
	DeleteFile(sTemph);

	char VolumeNameCopy[255];
	int iii=0;
	do
	{
	strcpy(VolumeNameCopy, sCDsPath);
	strcat(VolumeNameCopy, VolumeName);
	if(iii!=0)
	{
		if(fp)
			fclose(fp);
		char sss[255];
		itoa(iii, sss, 10);
		strcat(VolumeNameCopy, sss);
	}
	fp=fopen(strcat(VolumeNameCopy,".cdc"), "r");
	iii++;
	}
	while(fp);
	if(fp)
		fclose(fp);
	fp=fopen(VolumeNameCopy, "wb");
	DWORD dwVolumeNameSize=strlen(VolumeName);
	DWORD dwHeaderSize=3*sizeof(DWORD)+strlen(VolumeName);
	fwrite(&dwHeaderSize, sizeof(DWORD), 1,fp);
	fwrite(&dwVolumeNameSize, sizeof(DWORD), 1, fp);
	fwrite(VolumeName, sizeof(char)*dwVolumeNameSize, 1, fp);
	fwrite(&VolumeSerialNumber, sizeof(DWORD), 1, fp);

	strcpy(sTemph, sTmpPath);
	strcat(sTemph, "cd");
	fp1=fopen(sTemph, "rb");
	while(fread(&f, sizeof(f), 1, fp1)==1)
		fwrite(&f, sizeof(f), 1, fp);

	fclose(fp1);
	fclose(fp);

	strcpy(sTemph, sTmpPath);
	strcat(sTemph, "cd");
	DeleteFile(sTemph);

	return TRUE;
}

#include "resource.h"
void CfIlesys::sHowfolder(HWND hwndListView, LPCTSTR lpszPath)
{

   SHFILEINFO sfi;
   HIMAGELIST hilSmall, hilLarge;
   FILE* fp;

   SetCursor(::LoadCursor(NULL, IDC_WAIT));
   ListView_DeleteAllItems(hwndListView);
   fIlenode fn;
   long l= fIndpath(lpszPath);
   fp=oPen(iCurCD);
   if(!fp)
   {
	   SetCursor(::LoadCursor(NULL, IDC_ARROW));
	   return;
   }
   if(l==-1)
   {
	   curPath=new char[strlen(lpszPath)];
	   strcpy(curPath, lpszPath);
	   fclose(fp);
	   SetCursor(::LoadCursor(NULL, IDC_ARROW));
	   return;
   }
   fseek(fp, l, 0);
   fread(&fn, sizeof(fn), 1, fp);
   int i1st=fn.fthr;
   fseek(fp, l, 0);
   int i=0;
   long l2=ftell(fp);
   int nin=0;
   while((fread(&fn, sizeof(fn), 1, fp)==1)&&(fn.fthr==i1st))
	   nin++;
   fseek(fp, l2, 0);
	hilLarge=ListView_GetImageList(hwndListView, LVSIL_NORMAL);
	if(hilLarge)
		ImageList_RemoveAll(hilLarge);
	else
	{
		hilLarge=ImageList_Create(32, 32, ILC_COLOR8|ILC_MASK  ,0, nin+1);
		ListView_SetImageList(hwndListView, hilLarge, LVSIL_NORMAL);
	}
	hilSmall=ListView_GetImageList(hwndListView, LVSIL_SMALL);
	if(hilSmall)
		ImageList_RemoveAll(hilSmall);
	else
	{
		hilSmall=ImageList_Create(16, 16, ILC_COLOR8|ILC_MASK  ,0, nin+1);
		ListView_SetImageList(hwndListView, hilSmall, LVSIL_SMALL);
	}
   HICON hIcon=::LoadIcon((HINSTANCE)::GetWindowLong(::GetParent(hwndListView), GWL_HINSTANCE), MAKEINTRESOURCE(IDI_FOLDER));
   ImageList_AddIcon(hilLarge, hIcon);
   ImageList_AddIcon(hilSmall, hIcon);
   while((fread(&fn, sizeof(fn), 1, fp)==1)&&(fn.fthr==i1st))
   {
   ZeroMemory(&sfi,sizeof(sfi));
   /*hilSmall = (HIMAGELIST)*/SHGetFileInfo(fn.wfd.cFileName,
                       FILE_ATTRIBUTE_NORMAL,
                       &sfi,
                       sizeof(sfi),
                       SHGFI_SYSICONINDEX|SHGFI_USEFILEATTRIBUTES|SHGFI_ICON|SHGFI_LARGEICON);
   ImageList_AddIcon(hilLarge, sfi.hIcon);
   /*hilLarge = (HIMAGELIST)*/SHGetFileInfo(fn.wfd.cFileName,
                       FILE_ATTRIBUTE_NORMAL,
                       &sfi,
                       sizeof(sfi),
                       SHGFI_SYSICONINDEX|SHGFI_USEFILEATTRIBUTES|SHGFI_ICON|SHGFI_SMALLICON);
   int iImage=ImageList_AddIcon(hilSmall, sfi.hIcon);
//   ListView_SetImageList(hwndListView, hilSmall, LVSIL_SMALL);
//   ListView_SetImageList(hwndListView, hilLarge, LVSIL_NORMAL);
   LVITEM lvi;
   lvi.mask = LVIF_TEXT | LVIF_IMAGE;// | LVIF_PARAM;
   lvi.iItem       = i;
   lvi.iSubItem    = 0;
   lvi.pszText     = fn.wfd.cFileName;
   lvi.cchTextMax  = 255;
   if((fn.wfd.dwFileAttributes|FILE_ATTRIBUTE_DIRECTORY)==fn.wfd.dwFileAttributes)
		lvi.iImage      = 0;
   else
		lvi.iImage      = iImage/*sfi.iImage*/;
   if(ListView_InsertItem(hwndListView, &lvi)==-1)
	   MessageBox(NULL, "", "", MB_OK);
   i++;
   }
   fclose(fp);
   curPath=new char[strlen(lpszPath)];
   strcpy(curPath, lpszPath);
   SetCursor(::LoadCursor(NULL, IDC_ARROW));
}

long CfIlesys::fIndpath(LPCTSTR lpszPath)
{
   fIlenode f;
   char s[256];
   int i=0;
   int n=0;
   FILE* fp=oPen(iCurCD);
   if(!fp)
	   return -1;
   while(lpszPath[i]!='\0')
   {
	   if(i)
		   i++;
	   int j=i;
	   while((lpszPath[i]!='\\')&&(lpszPath[i]!='\0'))
		   s[i-j]=lpszPath[i++];
	   s[i-j]='\0';
	   BOOL bExit=FALSE;
	   while(!bExit)
	   {
		   bExit=(fread(&f, sizeof(f), 1, fp)==1);
		   n++;
		   bExit=bExit&&(strcmp(s, f.wfd.cFileName)==0);
	   }   
	   if(strcmp(s, f.wfd.cFileName)!=0)
	   {
		   fclose(fp);
		   return -1;
	   }
	   int end=f.chld1st;		
	   if(end==-1)
	   {
		   fclose(fp);
		   return -1;
	   }
	   while(n<end-1)
	   {
			if(fread(&f, sizeof(f), 1, fp)!=1)
				return -1;
			n++;
	   }
   }
   long lRet=ftell(fp);
   fclose(fp);
   return lRet;
}

BOOL bUildtRee(int fthr, LPCTSTR lpszPath, int& size, FILE* fp)
{	
	BOOL bMin2=FALSE;
	WIN32_FIND_DATA wfd;
	char st[255];
	strcpy(st, lpszPath);
	strcat(st,"*");
	HANDLE h=FindFirstFile(st, &wfd);
	if(h==INVALID_HANDLE_VALUE)
		return FALSE;
	int iNumber=0;
	do{	
		iNumber++;
	if((strcmp(wfd.cFileName,".")==0)||(strcmp(wfd.cFileName,"..")==0))
	{
		iNumber--;
		bMin2=TRUE;
	}
	}while(FindNextFile(h, &wfd));

	FindClose(h);

	if(iNumber==0)
		return FALSE;

	fIlenode* p=new fIlenode[iNumber];
	if(!p)
		return NULL;
	h=FindFirstFile(st, &p[0].wfd);
	int i=1;
	if(bMin2)
	{
		FindNextFile(h, &p[0].wfd);
		i=0;
	}
	for(; i<iNumber; i++)
		if(!FindNextFile(h, &p[i].wfd))
		{
			delete []p;
			return NULL;
		}

	FindClose(h);

	int iSize=size+iNumber;
	for(i=0; i<iNumber; i++)
	{
		p[i].fthr=fthr;
		fwrite(&p[i], sizeof(p[i]), 1, fp);
	}
	for(i=0; i<iNumber; i++)
	{
		if((p[i].wfd.dwFileAttributes|FILE_ATTRIBUTE_DIRECTORY)==p[i].wfd.dwFileAttributes)
		{
			char st[255];
			strcpy(st, lpszPath);
			strcat(st, p[i].wfd.cFileName);
			strcat(st, "\\");
			int it=iSize;
			bUildtRee(i+size, st, iSize, fp);
		}
	}
	size=iSize;
	delete []p;
	return TRUE;
}

int CfIlesys::oPen(int index, HWND hwnd)
{
   if(iCurCD==-1)
   {
	   iCurCD=index;
	   sHowfolder(hwnd, "");
	   return 0;
   }
   fIlenode fn;
   long l= fIndpath(curPath);
   if(l==-1)
	   return 1;
   FILE* fp=oPen(iCurCD);
   if(!fp)
	   return 1;
   fseek(fp, l, 0);
   fread(&fn, sizeof(fn), 1, fp);
   int i1st=fn.fthr;
   fseek(fp, l, 0);
   int i=0;
   while((fread(&fn, sizeof(fn), 1, fp)==1)&&(fn.fthr==i1st)&&(i<index))
	   i++;
   if((fn.wfd.dwFileAttributes|FILE_ATTRIBUTE_DIRECTORY)==fn.wfd.dwFileAttributes)
   {
	    char* sTemp=new char[strlen(curPath)+1+strlen(fn.wfd.cFileName)];
		if(strlen(curPath)!=0)
		{
			strcpy(sTemp, curPath);
			strcat(sTemp, "\\");
			strcat(sTemp, fn.wfd.cFileName);
		}
		else
			strcpy(sTemp, fn.wfd.cFileName);
		ListView_DeleteAllItems(hwnd);
		sHowfolder(hwnd, sTemp);
//		delete []sTemp;
   }
   else
   {
	   char sName[255];
	   if(cHeckcurCD(sName))
	   {
	    char* sTemp=new char[strlen(curPath)+1+strlen(fn.wfd.cFileName)];
		strcpy(sTemp, "x:\\");
		sTemp[0]=gEtcdromletter(1);
		if(strlen(curPath)!=0)
		{
			strcat(sTemp, curPath);
			strcat(sTemp, "\\");
			strcat(sTemp, fn.wfd.cFileName);
		}
		else
			strcpy(sTemp, fn.wfd.cFileName);
		int err;
		if((err=(int)ShellExecute(NULL, "open",sTemp, NULL, sTmpPath, SW_SHOW))==SE_ERR_NOASSOC)
			MessageBox(hwnd, "No program associated with this file type.", "Error", MB_OK | MB_ICONSTOP);
		else
			if(err<32)
				MessageBox(hwnd, "An error ocurred while trying to complete the operation.", "Error", MB_OK | MB_ICONSTOP);
//		delete []sTemp;
	   }
	   else
	   {
			char sMsg[255];
			strcpy(sMsg, "Wrong volume in the drive!\nPlease insert cdrom labeled ");
			strcat(sMsg, sName);
			strcat(sMsg, " in the drive and then try again.");
			MessageBox(hwnd, sMsg, "Error", MB_OK | MB_ICONSTOP);
	   }
   }
   fclose(fp);
   return 1;
}

int CfIlesys::bAck(HWND hwnd, HICON hIcon)
{
   int n=0;
   int i=0;
   if(iCurCD==-1)
	   return 0;
   if(!curPath)
	   return 0;
   if(strcmp(curPath, "")==0)
   {
	   ListView_DeleteAllItems(hwnd);
	   fIndCDContents(hwnd, hIcon);
	   return 0;
   }
  while(curPath[i]!='\0')
    {
	   n=i;
	   while((curPath[i]!='\\')&&(curPath[i]!='\0'))
			i++;
	   if(curPath[i]=='\\')
		   i++;
   }
   if(n>0)
	curPath[n-1]='\0';
   else
	curPath[0]='\0';
   sHowfolder(hwnd, curPath);
   return 1;
}

FILE* CfIlesys::oPen(int iCdID)
{
/*	WIN32_FIND_DATA wfd;
	char sPathh[255];
	strcpy(sPathh, sCDsPath);
	strcat(sPathh, "*.cdc");
	HANDLE h=FindFirstFile(sPathh, &wfd);
	if(h==INVALID_HANDLE_VALUE)
		return NULL;
	int i=0;
	while(i<iCdID)
	{
		if(!FindNextFile(h, &wfd))
		{
			FindClose(h);
			return NULL;
		}
		i++;
	}
	FindClose(h);
*/
	char sTemp[255];
	ScanCDs(FALSE, iCdID, sTemp);
	char sPathh[255];
	strcpy(sPathh, sCDsPath);
	strcat(sPathh, sTemp);
	FILE* fp=fopen(sPathh, "rb");
	if(!fp)
		return NULL;
	DWORD dwHeaderSize;
	fread(&dwHeaderSize, sizeof(DWORD), 1,fp);
	fseek(fp, dwHeaderSize*sizeof(BYTE), 0);
	return fp;
}

void CfIlesys::fIndCDContents(HWND hwndListView, HICON hIcon)
{
//	WIN32_FIND_DATA wfd;
	SHFILEINFO sfi;
	HIMAGELIST hilSmall, hilLarge;
//	int j=1;
	ListView_DeleteAllItems(hwndListView);
/*	char sPathh[255];
	strcpy(sPathh, sCDsPath);
	strcat(sPathh, "*.cdc");
	HANDLE h=FindFirstFile(sPathh, &wfd);
	if(h==INVALID_HANDLE_VALUE)
		return;
	while(FindNextFile(h, &wfd))
		j++;

	FindClose(h);*/
	char sTemp[255];
	ScanCDs(TRUE, -1, sTemp);

/*	h=FindFirstFile(sPathh, &wfd);
	if(h==INVALID_HANDLE_VALUE)
		return;*/
	if(iCDs==0)
		return;
	int i=0;
	hilLarge=ListView_GetImageList(hwndListView, LVSIL_NORMAL);
	if(hilLarge)
		ImageList_RemoveAll(hilLarge);
	else
	{
		hilLarge=ImageList_Create(32, 32, ILC_COLOR8|ILC_MASK  ,0, 1);
		ListView_SetImageList(hwndListView, hilLarge, LVSIL_NORMAL);
	}
	hilSmall=ListView_GetImageList(hwndListView, LVSIL_SMALL);
	if(hilSmall)
		ImageList_RemoveAll(hilSmall);
	else
	{
		hilSmall=ImageList_Create(16, 16, ILC_COLOR8|ILC_MASK  ,0, 1);
		ListView_SetImageList(hwndListView, hilSmall, LVSIL_SMALL);
	}
	ImageList_AddIcon(hilLarge, hIcon);
	ImageList_AddIcon(hilSmall, hIcon);
	do
	{
		ZeroMemory(&sfi,sizeof(sfi));
		LVITEM lvi;
		lvi.mask = LVIF_TEXT | LVIF_IMAGE ;//| LVIF_PARAM;
		lvi.iItem       = i;
		lvi.iSubItem    = 0;
		char s[150];
		strcpy(s, cds[i]/*wfd.cFileName*/);
		s[strlen(s)-4]='\0';
		lvi.pszText = s;
		lvi.cchTextMax  = 255;
		lvi.iImage      = 0;
		if(ListView_InsertItem(hwndListView, &lvi)==-1)
			MessageBox(NULL, "", "", MB_OK);
		i++;
	}
	while(i<iCDs/*FindNextFile(h, &wfd)*/);
//	FindClose(h);
	iCurCD=-1;
}

void CfIlesys::gEtCurPath(char* sPath)
{
	strcpy(sPath, "x:\\");
	sPath[0]=gEtcdromletter(1);
	strcat(sPath, curPath);
}

BOOL CfIlesys::cHeckcurCD(char* VolumeName2)
{
	char VolumeName[255];
	DWORD VolumeSerialNumber;
	char s[]="x:\\";
	int i=0;
	DWORD dwHeaderSize, dwVolumeNameSize, VolumeSerialNumber2;

	WIN32_FIND_DATA wfd;
	char sPathh[255];
	strcpy(sPathh, sCDsPath);
	strcat(sPathh, "*.cdc");
	HANDLE h=FindFirstFile(sPathh, &wfd);
	if(h==INVALID_HANDLE_VALUE)
		return NULL;
	while(i<iCurCD)
	{
		if(!FindNextFile(h, &wfd))
		{
			FindClose(h);
			return NULL;
		}
		i++;
	}
	FindClose(h);
	strcpy(sPathh, sCDsPath);
	strcat(sPathh, wfd.cFileName);
	FILE* fp=fopen(sPathh, "rb");
	if(!fp)
		return NULL;
	fread(&dwHeaderSize, sizeof(DWORD), 1,fp);
	fread(&dwVolumeNameSize, sizeof(DWORD), 1, fp);
	fread(VolumeName2, sizeof(char)*dwVolumeNameSize, 1, fp);
	VolumeName2[dwVolumeNameSize]='\0';
	fread(&VolumeSerialNumber2, sizeof(DWORD), 1, fp);
	s[0]=gEtcdromletter(1);
	if(!GetVolumeInformation(s, VolumeName, 255, &VolumeSerialNumber, NULL, NULL, NULL, 0))
		return FALSE;
	if(strcmp(VolumeName, VolumeName2)==0)
		return TRUE;
	return FALSE;
}

BOOL CfIlesys::Remove(int index)
{
	int i=0;
	WIN32_FIND_DATA wfd;
	char sPathh[255];
	strcpy(sPathh, sCDsPath);
	strcat(sPathh, "*.cdc");
	HANDLE h=FindFirstFile(sPathh, &wfd);
	if(h==INVALID_HANDLE_VALUE)
		return NULL;
	while(i<index)
	{
		if(!FindNextFile(h, &wfd))
		{
			FindClose(h);
			return FALSE;
		}
		i++;
	}
	FindClose(h);
	strcpy(sPathh, sCDsPath);
	strcat(sPathh, wfd.cFileName);
	return DeleteFile(sPathh);
}

void CfIlesys::FileProperties(int index, HWND hwnd)
{
   fIlenode fn;
   long l= fIndpath(curPath);
   if(l==-1)
	   return;
   FILE* fp=oPen(iCurCD);
   if(!fp)
	   return;
   fseek(fp, l, 0);
   fread(&fn, sizeof(fn), 1, fp);
   int i1st=fn.fthr;
   fseek(fp, l, 0);
   int i=0;
   while((fread(&fn, sizeof(fn), 1, fp)==1)&&(fn.fthr==i1st)&&(i<index))
	   i++;

   char sMsg[1000];
   strcpy(sMsg, "File Name:\t");
   strcat(sMsg, fn.wfd.cFileName);
   strcat(sMsg, "\nLocation:\t\t");
   char s[255];
   strcpy(s, "CDROM:\\");
//   s[0]=gEtcdromletter(1);
   strcat(sMsg, s);
   strcat(sMsg, curPath);
   char VolumeName[255];
   DWORD SerialNumber;
   GetDCInformation(VolumeName, SerialNumber);
   strcat(sMsg, "\nContainer Label:\t");
   strcat(sMsg, VolumeName);
   strcat(sMsg, "\nDOS File Name:\t");
   if(strlen(fn.wfd.cAlternateFileName)>0)
	strcat(sMsg, fn.wfd.cAlternateFileName);
   else
   {
	   strcpy(s, fn.wfd.cFileName);
	   for(int o=0; s[o]!='\0'; o++)
		   s[o]=toupper(s[o]);
		strcat(sMsg, s);
   }
   strcat(sMsg, "\n\nFile Size:\t\t");
   strcpy(s, curPath);
   if(strlen(curPath)>0)
	   strcat(s, "\\");
   strcat(s, fn.wfd.cFileName);
   long lSize=gEtsize(&fn, s);
   ltoa(lSize, s, 10);
   char a[255];
   strrev(s);
   int m=0;
   int n=-1;
   for(int k=0; s[k]!='\0'; k++)
   {
	   if(n==2)
	   {
		   a[m++]=',';
		   a[m]=s[k];
		   n=0;	
	   }
	   else
	   {
			a[m]=s[k];
			n++;
	   }
	m++;
   }
   a[m]='\0';
   strrev(a);
   strcat(sMsg, a);
   strcat(sMsg, " bytes");
   SYSTEMTIME SystemTime;
   strcat(sMsg, "\nCreated:\t\t");
   FileTimeToSystemTime(&fn.wfd.ftCreationTime, &SystemTime);
   SysTimeToStr(&SystemTime, s);
   strcat(sMsg, s);
   strcat(sMsg, "\nModified:\t\t");
   FileTimeToSystemTime(&fn.wfd.ftLastWriteTime, &SystemTime);
   SysTimeToStr(&SystemTime, s);
   strcat(sMsg, s);
   strcat(sMsg, "\n\nRead-Only:\t");
   if((fn.wfd.dwFileAttributes|FILE_ATTRIBUTE_READONLY)==fn.wfd.dwFileAttributes)
	   strcat(sMsg, "Yes");
   else
       strcat(sMsg, "No");
   strcat(sMsg, "\nArchive:\t\t");
   if((fn.wfd.dwFileAttributes|FILE_ATTRIBUTE_ARCHIVE)==fn.wfd.dwFileAttributes)
	   strcat(sMsg, "Yes");
   else
       strcat(sMsg, "No");
   strcat(sMsg, "\nHidden:\t\t");
   if((fn.wfd.dwFileAttributes|FILE_ATTRIBUTE_HIDDEN)==fn.wfd.dwFileAttributes)
	   strcat(sMsg, "Yes");
   else
       strcat(sMsg, "No");
   MessageBox(hwnd, sMsg, "File Info", MB_OK | MB_ICONINFORMATION);
   fclose(fp);
}

void CfIlesys::Properties(int index, HWND hwnd)
{
	char VolumeName[255];
	int i=0;
	DWORD dwHeaderSize, dwVolumeNameSize, VolumeSerialNumber;

	WIN32_FIND_DATA wfd;
	char sPathh[255];
	strcpy(sPathh, sCDsPath);
	strcat(sPathh, "*.cdc");
	HANDLE h=FindFirstFile(sPathh, &wfd);
	if(h==INVALID_HANDLE_VALUE)
		return;
	while(i<index)
	{
		if(!FindNextFile(h, &wfd))
		{
			FindClose(h);
			return;
		}
		i++;
	}
	FindClose(h);
	strcpy(sPathh, sCDsPath);
	strcat(sPathh, wfd.cFileName);
	FILE* fp=fopen(sPathh, "rb");
	if(!fp)
		return;
	fread(&dwHeaderSize, sizeof(DWORD), 1,fp);
	fread(&dwVolumeNameSize, sizeof(DWORD), 1, fp);
	fread(VolumeName, sizeof(char)*dwVolumeNameSize, 1, fp);
	VolumeName[dwVolumeNameSize]='\0';
	fread(&VolumeSerialNumber, sizeof(DWORD), 1, fp);
	fclose(fp);
	char a[255];
	ltoa(VolumeSerialNumber, a, 16);
	for(int j=0; a[j]!='\0'; j++)
		a[j]=toupper(a[j]);

	char sMsg[255];
	strcpy(sMsg, "File Container:\t");
	strcat(sMsg, wfd.cFileName);
	strcat(sMsg, "\nLabel:\t\t");
	strcat(sMsg, VolumeName);
	strcat(sMsg, "\nSerial Number:\t");
	strcat(sMsg, a);
	MessageBox(hwnd, sMsg, "Properties", MB_OK | MB_ICONINFORMATION);
}

void SysTimeToStr(LPSYSTEMTIME lpSystemTime, char* s)
{
   switch(lpSystemTime->wDayOfWeek)
   {
   case 0:
	   strcpy(s, "Sunday, ");
	   break;
   case 1:
	   strcpy(s, "Monday, ");
	   break;
   case 2:
	   strcpy(s, "Tuesday, ");
	   break;
   case 3:
	   strcpy(s, "Wednesday, ");
	   break;
   case 4:
	   strcpy(s, "Thursday, ");
	   break;
   case 5:
	   strcpy(s, "Friday, ");
	   break;
   case 6:
	   strcpy(s, "Saturday, ");
	   break;
   }
   switch(lpSystemTime->wMonth)
   {
   case 1:
	   strcat(s, "January ");
	   break;
   case 2:
	   strcat(s, "February ");
	   break;
   case 3:
	   strcat(s, "March ");
	   break;
   case 4:
	   strcat(s, "April ");
	   break;
   case 5:
	   strcat(s, "May ");
	   break;
   case 6:
	   strcat(s, "June ");
	   break;
   case 7:
	   strcat(s, "July ");
	   break;
   case 8:
	   strcat(s, "August ");
	   break;
   case 9:
	   strcat(s, "September ");
	   break;
   case 10:
	   strcat(s, "October ");
	   break;
   case 11:
	   strcat(s, "November ");
	   break;
   case 12:
	   strcat(s, "December ");
	   break;
   }
   char a[255];
   itoa(lpSystemTime->wDay, a, 10);
   strcat(s, a);
   strcat(s, ", ");
   itoa(lpSystemTime->wYear, a, 10);
   strcat(s, a);
   strcat(s, ", ");
   itoa(lpSystemTime->wHour, a, 10);
   strcat(s, a);
   strcat(s, ":");
   itoa(lpSystemTime->wMinute, a, 10);
   strcat(s, a);
   strcat(s, ":");
   itoa(lpSystemTime->wSecond, a, 10);
   strcat(s, a);
}

long CfIlesys::gEtsize(fIlenode* pfn, char *sFile)
{
   long lSize=0;
   if((pfn->wfd.dwFileAttributes|FILE_ATTRIBUTE_DIRECTORY)==pfn->wfd.dwFileAttributes)
   {
		long l=fIndpath(sFile);
		FILE* fp=oPen(iCurCD);
		fIlenode fn;
		fseek(fp, l, 0);
		fread(&fn, sizeof(fn), 1, fp);
		int i1st=fn.fthr;
		fseek(fp, l, 0);
		while((fread(&fn, sizeof(fn), 1, fp)==1)&&(fn.fthr==i1st))
		{
			char sTemp[255];
			strcpy(sTemp, sFile);
			strcat(sTemp, "\\");
			strcat(sTemp, fn.wfd.cFileName);
			lSize+=gEtsize(&fn, sTemp);
		}
		fclose(fp);
   }
   else
   {
	 lSize=(pfn->wfd.nFileSizeHigh * (MAXDWORD+1)) + pfn->wfd.nFileSizeLow; 		
   }
   return lSize;
}


void CfIlesys::GetDCInformation(char* VolumeName, DWORD& VolumeSerialNumber)
{
	int i=0;
	DWORD dwHeaderSize, dwVolumeNameSize;

	if(iCurCD==-1)
		return;

	WIN32_FIND_DATA wfd;
	char sPathh[255];
	strcpy(sPathh, sCDsPath);
	strcat(sPathh, "*.cdc");
	HANDLE h=FindFirstFile(sPathh, &wfd);
	if(h==INVALID_HANDLE_VALUE)
		return;
	while(i<iCurCD)
	{
		if(!FindNextFile(h, &wfd))
		{
			FindClose(h);
			return;
		}
		i++;
	}
	FindClose(h);
	strcpy(sPathh, sCDsPath);
	strcat(sPathh, wfd.cFileName);
	FILE* fp=fopen(sPathh, "rb");
	if(!fp)
		return;
	fread(&dwHeaderSize, sizeof(DWORD), 1,fp);
	fread(&dwVolumeNameSize, sizeof(DWORD), 1, fp);
	fread(VolumeName, sizeof(char)*dwVolumeNameSize, 1, fp);
	VolumeName[dwVolumeNameSize]='\0';
	//09/29/2005/ begins
	strcpy(VolumeName, wfd.cFileName);
	VolumeName[strlen(wfd.cFileName)-4]='\0';
	//09/29/2005/ ends

	fread(&VolumeSerialNumber, sizeof(DWORD), 1, fp);
	fclose(fp);
}

void CfIlesys::GetDCInformation(char* VolumeName, DWORD& VolumeSerialNumber, int index)
{
	int i=0;
	DWORD dwHeaderSize, dwVolumeNameSize;

	if(index==-1)
		return;

	WIN32_FIND_DATA wfd;
	char sPathh[255];
	strcpy(sPathh, sCDsPath);
	strcat(sPathh, "*.cdc");
	HANDLE h=FindFirstFile(sPathh, &wfd);
	if(h==INVALID_HANDLE_VALUE)
		return;
	while(i<index)
	{
		if(!FindNextFile(h, &wfd))
		{
			FindClose(h);
			return;
		}
		i++;
	}
	FindClose(h);
	strcpy(sPathh, sCDsPath);
	strcat(sPathh, wfd.cFileName);
	FILE* fp=fopen(sPathh, "rb");
	if(!fp)
		return;
	fread(&dwHeaderSize, sizeof(DWORD), 1,fp);
	fread(&dwVolumeNameSize, sizeof(DWORD), 1, fp);
	fread(VolumeName, sizeof(char)*dwVolumeNameSize, 1, fp);
	VolumeName[dwVolumeNameSize]='\0';
	//09/29/2005/ begins
	strcpy(VolumeName, wfd.cFileName);
	VolumeName[strlen(wfd.cFileName)-4]='\0';
	//09/29/2005/ ends
	fread(&VolumeSerialNumber, sizeof(DWORD), 1, fp);
	fclose(fp);
}

FILE* CfIlesys::fInd(char* sFile)
{
	char sTemp[255];
	GetTempFileName(sTmpPath, "CDS", 0, sTemp);
	FILE* fp=fopen(sTemp, "wb");
	if(fp)
	{
		WIN32_FIND_DATA wfd;
		char sPathh[255];
		strcpy(sPathh, sCDsPath);
		strcat(sPathh, "*.cdc");
		HANDLE h=FindFirstFile(sPathh, &wfd);
		if(h==INVALID_HANDLE_VALUE)
			return NULL;
		int index=0;
		iFound=0;
		do
		{
			char sLabel[255];
			DWORD dw;
			GetDCInformation(sLabel, dw, index);
			FILE* fpp=oPen(index);
			if(fpp != NULL)
			{
			fIlenode fn;
			long l=ftell(fpp);
			while(fread(&fn, sizeof(fn), 1, fpp)==1)
			{
				sEarchnode sn;
				char s1[255], s2[255];
				strcpy(s1, fn.wfd.cFileName);
				strcpy(s2, sFile);
				for(int x=0; s1[x]!='\0'; x++)
					s1[x]=toupper(s1[x]);
				for(int x=0; s2[x]!='\0'; x++)
					s2[x]=toupper(s2[x]);
				if(strstr(s1, s2)!=NULL)
				{
					sn.iCD=index;
					sn.l=l;
					strcpy(sn.sFileName, fn.wfd.cFileName);
					strcpy(sn.sLabel, sLabel);
					if((fn.wfd.dwFileAttributes|FILE_ATTRIBUTE_DIRECTORY)==fn.wfd.dwFileAttributes)
						sn.b=TRUE;
					else
						sn.b=FALSE;
					fwrite(&sn, sizeof(sn), 1, fp);
					iFound++;
				}
				l=ftell(fpp);
			}
			fclose(fpp);
			}
			index++;
		}
		while(FindNextFile(h, &wfd));
		FindClose(h);
		fclose(fp);
		fp=fopen(sTemp, "rb");
		strcpy(sFile, sTemp);		
		return fp;
	}
	return NULL;
}

 void CfIlesys::fIndPath(int iCdID, long l, char *sPath)
 {
 	FILE* fp=oPen(iCdID);
 	long lt=ftell(fp);
 	fseek(fp, l, 0);
 	fIlenode fn;
 	fread(&fn, sizeof(fn), 1, fp);
 	if(fn.fthr==-1)
 	{
 		fclose(fp);
 		strcpy(sPath, "");
 		return;
 	}
 	fseek(fp, lt, 0);
 	fIlenode fn2;
 	int i=0;
 	while((fread(&fn2, sizeof(fn2), 1, fp)==1)&&(fn2.fthr!=fn.fthr))
 		i++;
 	if(fn2.fthr!=fn.fthr)
 	{
 		fclose(fp);
 		return;
 	}
 	fseek(fp, lt, 0);
 	while((fread(&fn2, sizeof(fn2), 1, fp)==1)&&(fn2.chld1st!=i+1))
 		lt=ftell(fp);
 	if(fn2.chld1st!=i+1)
 	{
 		fclose(fp);
 		return;
 	}
 	fclose(fp);
 	char sTemp1[255];
 	strcpy(sTemp1, fn2.wfd.cFileName);
 	char sTemp2[255];
 	fIndPath(iCdID, lt, sTemp2);
 	if(strlen(sTemp2)>0)
 	{
 		strcat(sTemp2, "\\");
 		strcat(sTemp2, sTemp1);
		strcpy(sTemp1, sTemp2);
 	}
 	strcpy(sPath, sTemp1);
 }

void CfIlesys::SetCurCD(int index)
{
	iCurCD=index;
}

void CfIlesys::ScanCDs(BOOL bRescan, int index, char* sPath)
{
	static BOOL bScaned=FALSE;
	if((!bScaned)||(bRescan))
	{
		WIN32_FIND_DATA wfd;
		char sPathh[255];
		strcpy(sPathh, sCDsPath);
		strcat(sPathh, "*.cdc");
		iCDs=0;
		HANDLE h=FindFirstFile(sPathh, &wfd);
		if(h==INVALID_HANDLE_VALUE)
			return;
		do
		{
			strcpy(cds[iCDs], wfd.cFileName);
			iCDs++;
			if(iCDs>999)
			{
				MessageBox(NULL, "Sorry! But this program cannot handle more than 100 cd snapshots!", "Error", MB_OK);
				return;
			}
		}
		while(FindNextFile(h, &wfd));
		FindClose(h);
		bScaned=TRUE;
	}
	if(index!=-1)
		strcpy(sPath, cds[index]);
}