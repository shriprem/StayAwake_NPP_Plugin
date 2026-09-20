#include "StayAwakePanel.h"
#include "SelectKeyCodes.h"
#include "AboutDialog.h"

extern HINSTANCE _gModule;
SelectKeyCodes _selectKeyCodes;
AboutDialog _aboutDlg;


INT_PTR CALLBACK StayAwakePanel::run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam) {
   switch (message) {
   case WM_COMMAND:
      switch LOWORD(wParam) {

      case IDC_STAYAWAKE_KEYS_ROSTER_BTN:
         showSelectKeyCodesDialog();
         break;

      case IDC_STAYAWAKE_INTERVAL_MIN:
         if (HIWORD(wParam) == EN_KILLFOCUS)
            onKillFocusIntervalMin();
         break;

      case IDC_STAYAWAKE_INTERVAL_MAX:
         if (HIWORD(wParam) == EN_KILLFOCUS)
            onKillFocusIntervalMax();
         break;

      case IDC_STAYAWAKE_SET_INTERVAL_BTN:
         onSetInterval();
         break;

      case IDC_STAYAWAKE_STEALTH_MODE:
         StayAwakeStealthMode();
         break;

      case IDC_STAYAWAKE_PAUSE_RESUME_BTN:
         if (isTimerPaused())
            initAwakes();
         else
            pauseTimer();

         break;

      case IDCANCEL:
      case IDCLOSE:
         display(false);
         break;

      case IDC_STAYAWAKE_ABOUT_BUTTON:
         showAboutDialog();
         break;
      }

      break;

   case WM_TIMER:
      simulateAwakeKeyPress();
      break;

   case WM_LBUTTONDOWN:
   case WM_MBUTTONDOWN:
   case WM_RBUTTONDOWN:
      SetFocus(_hSelf);
      break;

   case WM_SETFOCUS:
      break;

   case WM_SIZE:
      onPanelResize(lParam);
      break;

   case WM_SHOWWINDOW:
      Utils::checkMenuItem(MI_STAY_AWAKE_PANEL, wParam);
      break;

   default:
      return DockingDlgInterface::run_dlgProc(message, wParam, lParam);
   }

   return FALSE;
}

void StayAwakePanel::initConfig() {
   NppMessage(NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, (LPARAM)sIniFilePath);
   PathAppend(sIniFilePath, PREF_INI_FILE);

   // Initialize RNG
   std::srand(static_cast<unsigned>(time(nullptr)));

   int nIntervalLegacy{}, nIntervalMin{}, nIntervalMax{};

   nIntervalLegacy = GetPrivateProfileInt(PREF_DEFAULTS, PREF_INTERVAL_LEGACY, DEF_PERIOD, sIniFilePath);
   if (nIntervalLegacy < MIN_PERIOD || nIntervalLegacy > MAX_PERIOD)
      nIntervalLegacy = DEF_PERIOD;

   nIntervalMin = GetPrivateProfileInt(PREF_DEFAULTS, PREF_INTERVAL_MINIMUM, nIntervalLegacy, sIniFilePath);
   if (nIntervalMin < MIN_PERIOD || nIntervalMin > MAX_PERIOD)
      nIntervalMin = DEF_PERIOD;

   nIntervalMax = GetPrivateProfileInt(PREF_DEFAULTS, PREF_INTERVAL_MAXIMUM, nIntervalLegacy, sIniFilePath);
   if (nIntervalMax < MIN_PERIOD || nIntervalMax > MAX_PERIOD)
      nIntervalMax = DEF_PERIOD;

   nIntervalMinSeconds = (nIntervalMin <= nIntervalMax) ? nIntervalMin : nIntervalMax;
   nIntervalMaxSeconds = (nIntervalMin >= nIntervalMax) ? nIntervalMin : nIntervalMax;
}

void StayAwakePanel::initPanel() {
   initConfig();

   hStealthMode = GetDlgItem(_hSelf, IDC_STAYAWAKE_STEALTH_MODE);
   hPauseResume = GetDlgItem(_hSelf, IDC_STAYAWAKE_PAUSE_RESUME_BTN);

   // Init Timer Seconds
   SetDlgItemInt(_hSelf, IDC_STAYAWAKE_INTERVAL_MIN, nIntervalMinSeconds, FALSE);
   SetDlgItemInt(_hSelf, IDC_STAYAWAKE_INTERVAL_MAX, nIntervalMaxSeconds, FALSE);

   Utils::addTooltip(_hSelf, IDC_STAYAWAKE_INTERVAL_MIN, L"", INTERVAL_TOOLTIP, 3, TRUE);
   Utils::addTooltip(_hSelf, IDC_STAYAWAKE_INTERVAL_MAX, L"", INTERVAL_TOOLTIP, 3, TRUE);

   SetWindowText(hPauseResume, isTimerPaused() ? BTN_TEXT_RESUME :BTN_TEXT_PAUSE);

   Utils::loadBitmap(_hSelf, IDC_STAYAWAKE_ABOUT_BUTTON, IDB_STAYAWAKE_ABOUT_BITMAP);
   Utils::addTooltip(_hSelf, IDC_STAYAWAKE_ABOUT_BUTTON, L"", ABOUT_DIALOG_TITLE, TRUE);

   if (isTimerPaused()) showPausedInfo(TRUE);
   bPanelInitialized = true;
}

wstring StayAwakePanel::getSelectedKeyCodes() {
   const int bufSize{ LEN_ROSTER_KEYCODES + 1 };
   wchar_t sBuf[bufSize]{};

   GetPrivateProfileString(PREF_DEFAULTS, PREF_SELECTED_KEYCODES, L"N/A", sBuf, bufSize, sIniFilePath);

   wstring sKeyCodes{ sBuf };

   if (sKeyCodes == L"N/A") {
      UINT nLegacyKeyCode = GetPrivateProfileInt(PREF_DEFAULTS, PREF_LEGACY_KEYCODE, 99, sIniFilePath);

      if (nLegacyKeyCode == 99)
         sKeyCodes = DEF_SELECTED_KEYCODES;
      else
      {
         sKeyCodes = wstring(LEN_ROSTER_KEYCODES, L'0');
         sKeyCodes.replace(nLegacyKeyCode % LEN_ROSTER_KEYCODES, 1, L"1");
      }

      WritePrivateProfileString(PREF_DEFAULTS, PREF_LEGACY_KEYCODE, nullptr, sIniFilePath);
      saveSelectedKeyCodes(sKeyCodes);
   }
   else if (!checkSelectedKeyCodes(sKeyCodes))
   {
      sKeyCodes = DEF_SELECTED_KEYCODES;
      saveSelectedKeyCodes(sKeyCodes);
   }

   return sKeyCodes;
}

bool StayAwakePanel::checkSelectedKeyCodes(wstring sKeyCodes) {
   return (sKeyCodes.length() == LEN_ROSTER_KEYCODES &&
      sKeyCodes != wstring(LEN_ROSTER_KEYCODES, L'0') &&
      sKeyCodes.find_first_not_of(L"01") == std::string::npos);
}

bool StayAwakePanel::saveSelectedKeyCodes(wstring sKeyCodes) {
   return checkSelectedKeyCodes(sKeyCodes) &&
      WritePrivateProfileString(PREF_DEFAULTS, PREF_SELECTED_KEYCODES, sKeyCodes.c_str(), sIniFilePath);
}

void StayAwakePanel::display(bool toShow) {
   DockingDlgInterface::display(toShow);

   panelMounted = toShow;

   if (toShow) {
      if (!isTimerPaused()) initAwakes();
      SetFocus(GetDlgItem(_hSelf, IDC_STAYAWAKE_INTERVAL_MIN));
   }
   else {
      if (_aboutDlg.isCreated() && _aboutDlg.isVisible())
         _aboutDlg.display(FALSE);
   }
}

void StayAwakePanel::setParent(HWND parent2set) {
   _hParent = parent2set;
}

void StayAwakePanel::showAboutDialog() {
   _aboutDlg.doDialog((HINSTANCE)_gModule);
}

wstring StayAwakePanel::getPreference(const wstring key, const wstring defaultVal) const {
   const int bufSize{ MAX_PATH };
   wstring ftBuf(bufSize, '\0');

   GetPrivateProfileString(PREF_DEFAULTS, key.c_str(), defaultVal.c_str(), ftBuf.data(), bufSize, sIniFilePath);

   return wstring{ ftBuf.c_str() };
}

bool StayAwakePanel::isTimerPaused() {
   return (getPreference(PREF_AWAKE_PAUSED, L"N") == L"Y");
}

void StayAwakePanel::showPausedInfo(bool both) {
   if (both)
      SetDlgItemText(_hSelf, IDC_STAYAWAKE_LAST_EVENT, L"Last StayAwake event:         PAUSED");

   SetDlgItemText(_hSelf, IDC_STAYAWAKE_NEXT_EVENT, L"Next StayAwake event:         PAUSED");
}

void StayAwakePanel::initRosterKeyCodes() {
   wstring sSelectedKeyCodes{ getSelectedKeyCodes() };

   nRosterLength = 0;

   for (int i{}; i < LEN_ROSTER_KEYCODES; i++) {
      if (sSelectedKeyCodes.at(i) == L'1')
         nRosterKeyCodes[nRosterLength++] = i;
   }
}

void StayAwakePanel::initAwakes() {
   initRosterKeyCodes();
   simulateAwakeKeyPress();

   if (bPanelInitialized)
      SetWindowText(hPauseResume, BTN_TEXT_PAUSE);

   WritePrivateProfileString(PREF_DEFAULTS, PREF_AWAKE_PAUSED, L"N", sIniFilePath);
}

void StayAwakePanel::killTimer() {
   KillTimer(_hSelf, nTimerID);
}

void StayAwakePanel::stealthMode(bool active) {
   if (active) {
      if (!isTimerPaused()) initAwakes();
   }
   else
      killTimer();

   CheckDlgButton(_hSelf, IDC_STAYAWAKE_STEALTH_MODE, active);
}

void StayAwakePanel::pauseTimer() {
   KillTimer(_hSelf, nTimerID);

   SetWindowText(hPauseResume, BTN_TEXT_RESUME);
   WritePrivateProfileString(PREF_DEFAULTS, PREF_AWAKE_PAUSED, L"Y", sIniFilePath);
   showPausedInfo(FALSE);
}

void StayAwakePanel::simulateAwakeKeyPress() {
   if (!nRosterLength) initRosterKeyCodes();

   UINT nAwakeKeyCode{ nRosterKeyCodes[rand() % nRosterLength] };
   wstring sAwakeKeyCode{};

   switch (nAwakeKeyCode) {
   case 1:
      sAwakeKeyCode = L"Volume Down && Up";
      keybd_event(VK_VOLUME_DOWN, 0, KEYEVENTF_EXTENDEDKEY | 0, 0);
      keybd_event(VK_VOLUME_DOWN, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
      Sleep(10);
      keybd_event(VK_VOLUME_UP, 0, KEYEVENTF_EXTENDEDKEY | 0, 0);
      keybd_event(VK_VOLUME_UP, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
      break;

   case 2:
   case 3:
   case 4:
   case 5:
   case 6:
   case 7:
   case 8:
   case 9:
   case 10:
   {
      sAwakeKeyCode = L"Unassigned Key #" + to_wstring(nAwakeKeyCode - 1);
      BYTE keycode{ static_cast<BYTE>(VK_UNASSIGNED_01 + nAwakeKeyCode - 2) };
      keybd_event(keycode, 0, KEYEVENTF_EXTENDEDKEY | 0, 0);
      keybd_event(keycode, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
      break;
   }

   case 11:
      sAwakeKeyCode = L"Unassigned Key #10";
      keybd_event(VK_UNASSIGNED_10, 0, KEYEVENTF_EXTENDEDKEY | 0, 0);
      keybd_event(VK_UNASSIGNED_10, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
      break;

   default:
      sAwakeKeyCode = L"Scroll Lock cycling";
      keybd_event(VK_SCROLL, 0, KEYEVENTF_EXTENDEDKEY | 0, 0);
      keybd_event(VK_SCROLL, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
      Sleep(10);
      keybd_event(VK_SCROLL, 0, KEYEVENTF_EXTENDEDKEY | 0, 0);
      keybd_event(VK_SCROLL, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
      break;

   }

   if (bPanelInitialized) {
      SYSTEMTIME lastTime{};
      GetLocalTime(&lastTime);
      SetDlgItemText(_hSelf, IDC_STAYAWAKE_LAST_EVENT, Utils::formatSystemTime(lastTime, L"Last StayAwake event").c_str());
      SetDlgItemText(_hSelf, IDC_STAYAWAKE_LAST_KEYCODE, (L"[" + sAwakeKeyCode + L"]").c_str());

      UINT nTimerSeconds{ nIntervalMinSeconds };
      if (nIntervalMinSeconds != nIntervalMaxSeconds)
         nTimerSeconds += rand() % (abs(static_cast<int>(nIntervalMaxSeconds - nIntervalMinSeconds)) + 1);

      nTimerID = SetTimer(_hSelf, nTimerID, nTimerSeconds * 1000, NULL);

      SYSTEMTIME nextTime{};
      GetSystemTime(&nextTime);
      Utils::addSecondsToTime(nextTime, nTimerSeconds);
      SetDlgItemText(_hSelf, IDC_STAYAWAKE_NEXT_EVENT, Utils::formatSystemTime(nextTime, L"Next StayAwake event").c_str());
   }
}

void StayAwakePanel::showSelectKeyCodesDialog() {
   if (_selectKeyCodes.doDialog((HINSTANCE)_gModule) == IDOK) {
      initRosterKeyCodes();
      if (!isTimerPaused()) simulateAwakeKeyPress();
   }
}

void StayAwakePanel::onKillFocusIntervalMin() {
   if (!bPanelInitialized) return;

   UINT nInterval{ GetDlgItemInt(_hSelf, IDC_STAYAWAKE_INTERVAL_MIN, nullptr, FALSE) };

   if (nInterval < MIN_PERIOD || nInterval > MAX_PERIOD)
   {
      Utils::showEditBalloonTip(GetDlgItem(_hSelf, IDC_STAYAWAKE_INTERVAL_MIN), INTERVAL_WARN_TITLE, INTERVAL_WARNING.c_str());
      SetDlgItemInt(_hSelf, IDC_STAYAWAKE_INTERVAL_MIN, nIntervalMinSeconds, FALSE);
      return;
   }

   nIntervalMinSeconds = nInterval;
}

void StayAwakePanel::onKillFocusIntervalMax() {
   if (!bPanelInitialized) return;

   UINT nInterval{ GetDlgItemInt(_hSelf, IDC_STAYAWAKE_INTERVAL_MAX, nullptr, FALSE) };

   if (nInterval < MIN_PERIOD || nInterval > MAX_PERIOD)
   {
      Utils::showEditBalloonTip(GetDlgItem(_hSelf, IDC_STAYAWAKE_INTERVAL_MAX), INTERVAL_WARN_TITLE, INTERVAL_WARNING.c_str());
      SetDlgItemInt(_hSelf, IDC_STAYAWAKE_INTERVAL_MAX, nIntervalMaxSeconds, FALSE);
      return;
   }

   nIntervalMaxSeconds = nInterval;
}

void StayAwakePanel::onSetInterval() {
   onKillFocusIntervalMin();
   onKillFocusIntervalMax();

   if (nIntervalMinSeconds > nIntervalMaxSeconds) {
      UINT nTemp{ nIntervalMinSeconds };
      nIntervalMinSeconds = nIntervalMaxSeconds;
      nIntervalMaxSeconds = nTemp;

      SetDlgItemInt(_hSelf, IDC_STAYAWAKE_INTERVAL_MIN, nIntervalMinSeconds, FALSE);
      SetDlgItemInt(_hSelf, IDC_STAYAWAKE_INTERVAL_MAX, nIntervalMaxSeconds, FALSE);
   }

   WritePrivateProfileString(PREF_DEFAULTS, PREF_INTERVAL_MINIMUM, to_wstring(nIntervalMinSeconds).c_str(), sIniFilePath);
   WritePrivateProfileString(PREF_DEFAULTS, PREF_INTERVAL_MAXIMUM, to_wstring(nIntervalMaxSeconds).c_str(), sIniFilePath);
   initAwakes();
}

void StayAwakePanel::onPanelResize(LPARAM lParam) {
   // About button
   HWND hAboutBtn{ GetDlgItem(_hSelf, IDC_STAYAWAKE_ABOUT_BUTTON) };
   RECT rcAboutBtn;
   GetWindowRect(hAboutBtn, &rcAboutBtn);

   int aboutBtnWidth{ rcAboutBtn.right - rcAboutBtn.left };
   int aboutBtnHeight{ rcAboutBtn.bottom - rcAboutBtn.top };

   MoveWindow(hAboutBtn, (LOWORD(lParam) - aboutBtnWidth - 3), (HIWORD(lParam) - aboutBtnHeight - 3), aboutBtnWidth, aboutBtnHeight, TRUE);
}
