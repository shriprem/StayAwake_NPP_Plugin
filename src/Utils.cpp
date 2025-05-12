#include "Utils.h"

extern HINSTANCE _gModule;
extern FuncItem pluginMenuItems[MI_COUNT];

void Utils::addSecondsToTime(SYSTEMTIME& st, int seconds) {
   FILETIME ft{}, ftLoc{};
   SystemTimeToFileTime(&st, &ft);

   ULARGE_INTEGER uli{};
   uli.LowPart = ft.dwLowDateTime;
   uli.HighPart = ft.dwHighDateTime;

   // Add seconds (convert to 100-nanosecond intervals)
   uli.QuadPart += static_cast<ULONGLONG>(seconds) * 10000000ULL;

   ft.dwLowDateTime = uli.LowPart;
   ft.dwHighDateTime = uli.HighPart;

   FileTimeToLocalFileTime(&ft, &ftLoc);
   FileTimeToSystemTime(&ftLoc, &st);
}

wstring Utils::formatSystemTime(SYSTEMTIME& st, wstring prefix) {
   wstring sFormatted(100, '\0');
   swprintf(sFormatted.data(), 100, L"%s at: %02d:%02d:%02d %s",
      prefix.c_str(), st.wHour % 12 + ((st.wHour % 12) == 0 ? 12 : 0), st.wMinute, st.wSecond, (st.wHour < 12 ? L"am" : L"pm"));
   return sFormatted;
}

HWND Utils::addTooltip(HWND hDlg, int controlID, const wstring& pTitle, const wstring& pMessage, bool bBalloon) {
   if (!controlID || !hDlg || pMessage.empty())
      return FALSE;

   // Get the window of the tool.
   HWND hwndCtrl = GetDlgItem(hDlg, controlID);

   UINT ttsBalloon = bBalloon ? TTS_BALLOON : NULL;

   // Create the tooltip.
   HWND hwndTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,
      WS_POPUP | TTS_ALWAYSTIP | ttsBalloon,
      CW_USEDEFAULT, CW_USEDEFAULT,
      CW_USEDEFAULT, CW_USEDEFAULT,
      hDlg, NULL,
      _gModule, NULL);

   if (!hwndCtrl || !hwndTip)
      return (HWND)NULL;

   // Associate the tooltip with the tool.
   TOOLINFO toolInfo{};
   toolInfo.cbSize = sizeof(toolInfo);
   toolInfo.hwnd = hDlg;
   toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
   toolInfo.uId = (UINT_PTR)hwndCtrl;
   toolInfo.lpszText = (LPWSTR)pMessage.c_str();
   SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);
   SendMessage(hwndTip, TTM_SETTITLE, TTI_INFO, (LPARAM)pTitle.c_str());
   SendMessage(hwndTip, TTM_SETMAXTIPWIDTH, 0, (LPARAM)PREFS_TIP_MAX_WIDTH);

   return hwndTip;
}

HWND Utils::addTooltip(HWND hDlg, int controlID, const wstring& pTitle, const wstring& pMessage, int duration, bool bBalloon) {
   HWND hwndTip{ addTooltip(hDlg, controlID, pTitle, pMessage, bBalloon) };
   SendMessage(hwndTip, TTM_SETDELAYTIME, TTDT_AUTOPOP, (LPARAM)(duration * 1000));
   return hwndTip;
}

void Utils::checkMenuItem(int menuIndex, bool check) {
   nppMessage(NPPM_SETMENUITEMCHECK, pluginMenuItems[menuIndex]._cmdID, check);
}

void Utils::showEditBalloonTip(HWND hEdit, LPCWSTR title, LPCWSTR text) {
   EDITBALLOONTIP tip{};

   tip.cbStruct = sizeof(tip);
   tip.pszTitle = title;
   tip.pszText = text;
   tip.ttiIcon = TTI_ERROR;

   SendMessage(hEdit, EM_SHOWBALLOONTIP, NULL, (LPARAM)&tip);
   MessageBeep(MB_OK);
}

wstring Utils::getVersionInfo(LPCWSTR key) {
   wstring sVersionInfo;
   wchar_t sModuleFilePath[MAX_PATH];
   DWORD  verHandle{};
   DWORD  verSize{};
   UINT   querySize{};
   LPTSTR lpBuffer{};

   struct LANGANDCODEPAGE {
      WORD wLanguage;
      WORD wCodePage;
   } *lpTranslate{};

   GetModuleFileName(_gModule, sModuleFilePath, MAX_PATH);
   verSize = GetFileVersionInfoSize(sModuleFilePath, &verHandle);
   if (verSize < 1) return L"";

   string versionData(verSize, '\0');
   if (!GetFileVersionInfo(sModuleFilePath, NULL, verSize, versionData.data())) return L"";
   VerQueryValue(versionData.data(), L"\\VarFileInfo\\Translation", (VOID FAR* FAR*)& lpTranslate, &querySize);

   wstring verSubBlock(100, '\0');
   swprintf(verSubBlock.data(), 100, L"\\StringFileInfo\\%04X%04X\\%s",
      lpTranslate[0].wLanguage, lpTranslate[0].wCodePage, key);

   if (VerQueryValue(versionData.data(), verSubBlock.c_str(), (VOID FAR* FAR*)& lpBuffer, &querySize)
      && querySize)
      sVersionInfo = wstring{ lpBuffer };

   return sVersionInfo;
}

void Utils::loadBitmap(HWND hDlg, int controlID, int resource) {
   HWND hwndCtrl = GetDlgItem(hDlg, controlID);
   HBITMAP hBitmap = LoadBitmap(_gModule, MAKEINTRESOURCE(resource));

   if (hBitmap) {
      SendMessage(hwndCtrl, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
      DeleteObject(hBitmap);
   }
}
