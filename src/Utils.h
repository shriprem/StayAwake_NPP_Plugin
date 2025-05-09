#pragma once

#include "PluginDefinition.h"
#include <ShlObj_core.h>

#define PREFS_TIP_MAX_WIDTH         400

namespace Utils {
   void addSecondsToTime(SYSTEMTIME& st, int seconds);
   wstring formatSystemTime(SYSTEMTIME& st, wstring prefix);

   HWND addTooltip(HWND hDlg, int controlID, const wstring& pTitle, const wstring& pMessage, bool bBalloon = TRUE);
   HWND addTooltip(HWND hDlg, int controlID, const wstring& pTitle, const wstring& pMessage, int duration, bool bBalloon);
   void checkMenuItem(int menuIndex, bool check);
   void showEditBalloonTip(HWND hEdit, LPCWSTR title, LPCWSTR text);
   wstring getVersionInfo(LPCWSTR key);
   void loadBitmap(HWND hDlg, int controlID, int resource);
}
