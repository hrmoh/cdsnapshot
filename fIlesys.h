#if !defined(__fIlesys_h)
#define __fIlesys_h

#include "sNapshot.h"
#include <stdio.h>

extern char sCDsPath[255];
extern char sTmpPath[255];


struct fIlenode
{
	WIN32_FIND_DATA wfd;
	int chld1st;
	int fthr;	
	fIlenode(){chld1st=-1;}
};

struct sEarchnode
{
	char sFileName[256];
	int iCD;
	char sLabel[30];
	long l;
	BOOL b;
};

BOOL bUildtRee(int fthr, LPCTSTR lpszPath, int& size, FILE* fp);

class CfIlesys
{
public:
	void Init();
	char gEtcdromletter(int index);
	BOOL bUildcdtRee();
	void sHowfolder(HWND hwndListView, LPCTSTR lpszPath);
	long fIndpath(LPCTSTR lpszPath);
	int oPen(int index, HWND hwnd);
	int bAck(HWND hwnd, HICON hIcon);
	FILE* oPen(int iCdID);
	void fIndCDContents(HWND hwndListView, HICON hIcon);
	int GetCurCD(){return iCurCD;}
	void gEtCurPath(char* sPath);
	BOOL cHeckcurCD(char* VolumeName2);
	BOOL Remove(int index);
	void Properties(int index, HWND hwnd);
	void FileProperties(int index, HWND hwnd);
	long gEtsize(fIlenode* pfn, char *sFile);
	void GetDCInformation(char* VolumeName, DWORD& VolumeSerialNumber);
	void GetDCInformation(char* VolumeName, DWORD& VolumeSerialNumber, int index);
	FILE* fInd(char* sFile);
	void fIndPath(int iCdID, long l, char *sPath);
    void SetCurCD(int index);
	void ScanCDs(BOOL bRescan, int index, char* sPath);

private:
	int iTreeSize;
	char *curPath;
	int iCurCD;
	char cds[1000][255];
	int iCDs;
};
#endif