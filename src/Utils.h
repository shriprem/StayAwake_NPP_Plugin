#pragma once

#include "PluginDefinition.h"
#include <codecvt>
#include <regex>
#include <ShlObj_core.h>

#define PREFS_TIP_MAX_WIDTH         400
#define mbox(message)               MessageBox(NULL, message, L"", MB_OK)
#define mboxA(message)              MessageBoxA(NULL, message, "", MB_OK)

namespace Utils {
   void addSecondsToTime(SYSTEMTIME& st, int seconds);
   wstring formatSystemTime(SYSTEMTIME& st, wstring prefix);

   COLORREF intToRGB(int color);
   int scaleDPIX(int x);
   int scaleDPIY(int y);
   wstring getSpecialFolder(int folderID);
   wstring getKnownFolderPath(REFKNOWNFOLDERID folderID);

   HWND addTooltip(HWND hDlg, int controlID, const wstring& pTitle, const wstring& pMessage, bool bBalloon = TRUE);
   HWND addTooltip(HWND hDlg, int controlID, const wstring& pTitle, const wstring& pMessage, int duration, bool bBalloon);
   void addToolbarIcon(int menuIndex, int std, int fluent, int dark);
   void checkMenuItem(int menuIndex, bool check);
   void showEditBalloonTip(HWND hEdit, LPCWSTR title, LPCWSTR text);
   bool checkBaseOS(winVer os);
   float getNPPVersion();
   wstring getVersionInfo(LPCWSTR key);
   void loadBitmap(HWND hDlg, int controlID, int resource);

   bool checkDirectoryExists(LPCWSTR lpDirPath);
   bool checkFileExists(LPCWSTR lpFilePath);
}
