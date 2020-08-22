#if !defined(__sNapshotwNd_h)
#define __sNapshotwNd_h

#include <windows.h>
#include "fIlesys.h"

#define SUCCESS 0
#define NO_SUCCESS 1
#define NO_INITIALIZATION 2

class CsNapshotWnd{
	private:

	HINSTANCE m_hInstance;
		HMENU m_hMenu;
	 	 HWND m_hwnd;
		 HWND m_hwndListView;
		 HWND m_hwndToolbar;

		 HWND m_hAboutDlg;
		 HWND m_hOptionsDlg;
		 HWND m_hFindDlg;


	   void InitializeMenu();
       void CreateListView();	   
       void CreateToolbar();	   

	public:
	   void Initialize(HINSTANCE hInstance);
		int InitializeClass(HINSTANCE hPrevInstance);
		int InitializeWindow();	   	

		HWND GetWindow(){return m_hwnd;}

		void OnExit();
		void OnAbout();
		void OnOptions();
		void OnSize();
		void OnNew();
		void OnUp();
		void OnLButtonDblClk();
		void OnSwitchView(DWORD dwView);
		void OnRClick(HWND hwndFrom, int iItem ,int x, int y);
		void OpenInExplorer(char* sAction);
		void OnRemove();
		void OnProperties();
		void OnFileProperties();
		void ReInit();
		void OnFind();
		void UpdateCaption();

		BOOL IsDialogMessage(LPMSG lpMsg);
};
#endif