#include "StayAwakePanel.h"
#include "SelectKeyCodes.h"
#include "AboutDialog.h"

extern HINSTANCE _gModule;
SelectKeyCodes _selectKeyCodes;
AboutDialog _aboutDlg;


INT_PTR CALLBACK StayAwakePanel::run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam) {
   switch (message) {
   case WM_INITDIALOG:
      WTSRegisterSessionNotification(_hSelf, NOTIFY_FOR_THIS_SESSION);
      break;

   case WM_COMMAND:
      switch LOWORD(wParam) {

      case IDC_SELECT_INPUT_OPTIONS_BTN:
         showSelectKeyCodesDialog();
         break;

      case IDC_INTERVAL_MIN_EDIT:
         if (HIWORD(wParam) == EN_KILLFOCUS)
            onKillFocusIntervalMin();
         break;

      case IDC_INTERVAL_MAX_EDIT:
         if (HIWORD(wParam) == EN_KILLFOCUS)
            onKillFocusIntervalMax();
         break;

      case IDC_SET_TIMER_BTN:
         onSetInterval();
         break;

      case IDC_STEALTH_MODE_CBX:
         StayAwakeStealthMode();
         break;

      case IDC_PAUSE_RESUME_BTN:
         if (isTimerPaused())
            initAwakes();
         else
            pauseTimer();

         break;

      case IDCANCEL:
      case IDCLOSE:
         display(false);
         break;

      case IDC_ABOUT_BTN:
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

   case WM_WTSSESSION_CHANGE:
      switch (wParam)
      {
      case WTS_SESSION_LOCK:
         bSystemLocked = true;
         break;

      case WTS_SESSION_UNLOCK:
         bSystemLocked = false;
         break;
      }
      break;

   case WM_DESTROY:
         WTSUnRegisterSessionNotification(_hSelf);
         break;

   default:
      return DockingDlgInterface::run_dlgProc(message, wParam, lParam);
   }

   return FALSE;
}

void StayAwakePanel::initConfig() {
   wchar_t sIniFilePath[MAX_PATH]{};
   NppMessage(NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, (LPARAM)sIniFilePath);
   PathAppend(sIniFilePath, PREF_INI_FILE);
   mAwakeCore.SetConfigFilePath(sIniFilePath);

   // Initialize RNG
   std::srand(static_cast<unsigned>(time(nullptr)));

   mAwakeCore.InitIntervals(nIntervalMinSeconds, nIntervalMaxSeconds);
}

void StayAwakePanel::initPanel() {
   initConfig();

   hStealthMode = GetDlgItem(_hSelf, IDC_STEALTH_MODE_CBX);
   hPauseResume = GetDlgItem(_hSelf, IDC_PAUSE_RESUME_BTN);

   // Init Timer Seconds
   SetDlgItemInt(_hSelf, IDC_INTERVAL_MIN_EDIT, nIntervalMinSeconds, FALSE);
   SetDlgItemInt(_hSelf, IDC_INTERVAL_MAX_EDIT, nIntervalMaxSeconds, FALSE);

   Utils::addTooltip(_hSelf, IDC_INTERVAL_MIN_EDIT, L"", INTERVAL_TOOLTIP, 3, TRUE);
   Utils::addTooltip(_hSelf, IDC_INTERVAL_MAX_EDIT, L"", INTERVAL_TOOLTIP, 3, TRUE);

   SetWindowText(hPauseResume, isTimerPaused() ? BTN_TEXT_RESUME :BTN_TEXT_PAUSE);

   Utils::loadBitmap(_hSelf, IDC_ABOUT_BTN, IDB_STAYAWAKE_ABOUT_BITMAP);
   Utils::addTooltip(_hSelf, IDC_ABOUT_BTN, L"", ABOUT_DIALOG_TITLE, TRUE);

   if (isTimerPaused()) showPausedInfo(TRUE);
   bPanelInitialized = true;
}

void StayAwakePanel::display(bool toShow) {
   DockingDlgInterface::display(toShow);

   panelMounted = toShow;

   if (toShow) {
      if (!isTimerPaused()) initAwakes();
      SetFocus(GetDlgItem(_hSelf, IDC_INTERVAL_MIN_EDIT));
   }
}

void StayAwakePanel::setParent(HWND parent2set) {
   _hParent = parent2set;
}

void StayAwakePanel::showAboutDialog() {
   _aboutDlg.doDialog((HINSTANCE)_gModule);
}

bool StayAwakePanel::isTimerPaused() {
   return (mAwakeCore.GetPreference(PREF_AWAKE_PAUSED, L"N") == L"Y");
}

void StayAwakePanel::showPausedInfo(bool both) {
   if (both)
      SetDlgItemText(_hSelf, IDC_LAST_EVENT_TIME_INFO, L"Last StayAwake event:         PAUSED");

   SetDlgItemText(_hSelf, IDC_NEXT_EVENT_TIME_INFO, L"Next StayAwake event:         PAUSED");
}

void StayAwakePanel::initRosterKeyCodes() {
   wstring sSelectedKeyCodes{ mAwakeCore.GetSelectedKeyCodes() };

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

   mAwakeCore.SetPreference(PREF_AWAKE_PAUSED, L"N");
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

   CheckDlgButton(_hSelf, IDC_STEALTH_MODE_CBX, active);
}

void StayAwakePanel::pauseTimer() {
   KillTimer(_hSelf, nTimerID);

   SetWindowText(hPauseResume, BTN_TEXT_RESUME);
   mAwakeCore.SetPreference(PREF_AWAKE_PAUSED, L"Y");
   showPausedInfo(FALSE);
}

void StayAwakePanel::simulateAwakeKeyPress() {
   if (!nRosterLength) initRosterKeyCodes();

   if (bSystemLocked)
   {
      SetDlgItemText(_hSelf, IDC_NEXT_EVENT_TIME_INFO, L"PAUSED since Windows is LOCKED");
      return;
   }

   UINT nAwakeKeyCode{ nRosterKeyCodes[rand() % nRosterLength] };
   wstring sAwakeKeyCode{};

   mAwakeCore.SimulateInput(nAwakeKeyCode, sAwakeKeyCode);

   UINT nTimerSeconds{ nIntervalMinSeconds };
   if (nIntervalMinSeconds != nIntervalMaxSeconds)
      nTimerSeconds += rand() % (abs(static_cast<int>(nIntervalMaxSeconds - nIntervalMinSeconds)) + 1);

   nTimerID = SetTimer(_hSelf, nTimerID, nTimerSeconds * 1000, NULL);

   if (bPanelInitialized) {
      SYSTEMTIME lastTime{};
      GetLocalTime(&lastTime);
      SetDlgItemText(_hSelf, IDC_LAST_EVENT_TIME_INFO, Utils::formatSystemTime(lastTime, L"Last StayAwake event").c_str());

#ifdef DEBUG_DISPLAY_IDLE_TICKS
      Sleep((rand() % 20) + 1); // simulate a small delay to get a more accurate idle time
      ULONGLONG idleTime = mAwakeCore.GetIdleTimeMilliseconds();
      SetDlgItemText(_hSelf, IDC_LAST_EVENT_INPUT_INFO, (L"[" + sAwakeKeyCode + L":" + to_wstring(idleTime) + L" ms]").c_str());
#else
      SetDlgItemText(_hSelf, IDC_LAST_EVENT_INPUT_INFO, (L"[" + sAwakeKeyCode + L"]").c_str());
#endif

      SYSTEMTIME nextTime{};
      GetSystemTime(&nextTime);
      Utils::addSecondsToTime(nextTime, nTimerSeconds);
      SetDlgItemText(_hSelf, IDC_NEXT_EVENT_TIME_INFO, Utils::formatSystemTime(nextTime, L"Next StayAwake event").c_str());
   }
}

void StayAwakePanel::showSelectKeyCodesDialog() {
   if (_selectKeyCodes.doDialog((HINSTANCE)_gModule, mAwakeCore) == IDOK) {
      initRosterKeyCodes();
      if (!isTimerPaused()) simulateAwakeKeyPress();
   }
}

void StayAwakePanel::onKillFocusIntervalMin() {
   if (!bPanelInitialized) return;

   UINT nInterval{ GetDlgItemInt(_hSelf, IDC_INTERVAL_MIN_EDIT, nullptr, FALSE) };

   if (nInterval < MIN_PERIOD || nInterval > MAX_PERIOD)
   {
      Utils::showEditBalloonTip(GetDlgItem(_hSelf, IDC_INTERVAL_MIN_EDIT), INTERVAL_WARN_TITLE, INTERVAL_WARNING.c_str());
      SetDlgItemInt(_hSelf, IDC_INTERVAL_MIN_EDIT, nIntervalMinSeconds, FALSE);
      return;
   }

   nIntervalMinSeconds = nInterval;
}

void StayAwakePanel::onKillFocusIntervalMax() {
   if (!bPanelInitialized) return;

   UINT nInterval{ GetDlgItemInt(_hSelf, IDC_INTERVAL_MAX_EDIT, nullptr, FALSE) };

   if (nInterval < MIN_PERIOD || nInterval > MAX_PERIOD)
   {
      Utils::showEditBalloonTip(GetDlgItem(_hSelf, IDC_INTERVAL_MAX_EDIT), INTERVAL_WARN_TITLE, INTERVAL_WARNING.c_str());
      SetDlgItemInt(_hSelf, IDC_INTERVAL_MAX_EDIT, nIntervalMaxSeconds, FALSE);
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

      SetDlgItemInt(_hSelf, IDC_INTERVAL_MIN_EDIT, nIntervalMinSeconds, FALSE);
      SetDlgItemInt(_hSelf, IDC_INTERVAL_MAX_EDIT, nIntervalMaxSeconds, FALSE);
   }

   mAwakeCore.SetPreference(PREF_INTERVAL_MINIMUM, to_wstring(nIntervalMinSeconds));
   mAwakeCore.SetPreference(PREF_INTERVAL_MAXIMUM, to_wstring(nIntervalMaxSeconds));
   initAwakes();
}

void StayAwakePanel::onPanelResize(LPARAM lParam) {
   // About button
   HWND hAboutBtn{ GetDlgItem(_hSelf, IDC_ABOUT_BTN) };
   RECT rcAboutBtn;
   GetWindowRect(hAboutBtn, &rcAboutBtn);

   int aboutBtnWidth{ rcAboutBtn.right - rcAboutBtn.left };
   int aboutBtnHeight{ rcAboutBtn.bottom - rcAboutBtn.top };

   MoveWindow(hAboutBtn, (LOWORD(lParam) - aboutBtnWidth - 3), (HIWORD(lParam) - aboutBtnHeight - 3), aboutBtnWidth, aboutBtnHeight, TRUE);
}
