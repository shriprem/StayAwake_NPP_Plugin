#include "StayAwakePanel.h"
#include "AboutDialog.h"

extern HINSTANCE _gModule;
AboutDialog _aboutDlg;


constexpr auto PREF_INI_FILE = L"StayAwake.ini";
constexpr auto PREF_DEFAULTS = L"Defaults";
constexpr auto PREF_AWAKE_KEYCODE = L"AwakeKeyCode";
constexpr auto PREF_AWAKE_PAUSED = L"AwakePaused";
constexpr auto PREF_INTERVAL_LEGACY = L"TimerIntervalInSeconds";
constexpr auto PREF_INTERVAL_MINIMUM = L"MinimumIntervalInSeconds";
constexpr auto PREF_INTERVAL_MAXIMUM = L"MaximumIntervalInSeconds";

constexpr auto BTN_TEXT_PAUSE = L"&Pause";
constexpr auto BTN_TEXT_RESUME = L"&Resume";

constexpr auto VK_UNASSIGNED_01 = 0x97;
constexpr auto VK_UNASSIGNED_10 = 0xE8;

constexpr auto MIN_PERIOD{ 10 };
constexpr auto MAX_PERIOD{ 9990 };
constexpr auto DEF_PERIOD{ 240 };

const wstring MIN_MAX_PERIOD = to_wstring(MIN_PERIOD) + L" and " + to_wstring(MAX_PERIOD);
const wstring INTERVAL_TOOLTIP = L"Number between " + MIN_MAX_PERIOD;
const wstring INTERVAL_WARNING = L"Please enter a value between " + MIN_MAX_PERIOD;
const LPCWSTR INTERVAL_WARN_TITLE = L"Timer Interval in seconds";


INT_PTR CALLBACK StayAwakePanel::run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam) {
   switch (message) {
   case WM_COMMAND:
      switch LOWORD(wParam) {

      case IDC_STAYAWAKE_KEY_LIST:
         switch HIWORD(wParam) {
         case CBN_SELCHANGE:
            nAwakeKeyCode = static_cast<int>(SendMessage(hKeyCodes, CB_GETCURSEL, 0, 0));
            nAwakeKeyCode %= 12;
            WritePrivateProfileString(PREF_DEFAULTS, PREF_AWAKE_KEYCODE, to_wstring(nAwakeKeyCode).c_str(), sIniFilePath);
            break;
         }
         break;

      case IDC_STAYAWAKE_INTERVAL_MIN:
         if (HIWORD(wParam) == EN_KILLFOCUS)
            onKillFocusIntervalMin();
         break;

      case IDC_STAYAWAKE_INTERVAL_MAX:
         if (HIWORD(wParam) == EN_KILLFOCUS)
            onKillFocusIntervalMax();
         break;

      case IDC_STAYAWAKE_SET_INTERVAL:
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

   nAwakeKeyCode = GetPrivateProfileInt(PREF_DEFAULTS, PREF_AWAKE_KEYCODE, nAwakeKeyCode, sIniFilePath);
   nAwakeKeyCode %= 12;

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

   hKeyCodes = GetDlgItem(_hSelf, IDC_STAYAWAKE_KEY_LIST);
   hStealthMode = GetDlgItem(_hSelf, IDC_STAYAWAKE_STEALTH_MODE);
   hPauseResume = GetDlgItem(_hSelf, IDC_STAYAWAKE_PAUSE_RESUME_BTN);

   // Init KeyCodes List
   SendMessage(hKeyCodes, CB_ADDSTRING, NULL, (LPARAM)L"Scroll Lock cycling");
   SendMessage(hKeyCodes, CB_ADDSTRING, NULL, (LPARAM)L"Volume Down & Up");

   for (int i{ 1 }; i <= 10; i++) {
      SendMessage(hKeyCodes, CB_ADDSTRING, NULL, (LPARAM)(L"Unassigned Key #" + to_wstring(i)).c_str());
   }

   SendMessage(hKeyCodes, CB_SETCURSEL, nAwakeKeyCode, NULL);

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
      SetDlgItemText(_hSelf, IDC_STAYAWAKE_LAST_TOGGLE, L"Last StayAwake event:         PAUSED");

   SetDlgItemText(_hSelf, IDC_STAYAWAKE_NEXT_TOGGLE, L"Next StayAwake event:         PAUSED");
}

void StayAwakePanel::initAwakes() {
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
   switch (nAwakeKeyCode) {
   case 1:
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
      BYTE keycode{ static_cast<BYTE>(VK_UNASSIGNED_01 + nAwakeKeyCode - 2) };
      keybd_event(keycode, 0, KEYEVENTF_EXTENDEDKEY | 0, 0);
      keybd_event(keycode, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
      break;
   }

   case 11:
      keybd_event(VK_UNASSIGNED_10, 0, KEYEVENTF_EXTENDEDKEY | 0, 0);
      keybd_event(VK_UNASSIGNED_10, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
      break;

   default:
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
      SetDlgItemText(_hSelf, IDC_STAYAWAKE_LAST_TOGGLE, Utils::formatSystemTime(lastTime, L"Last StayAwake event").c_str());

      UINT nTimerSeconds{ nIntervalMinSeconds };
      if (nIntervalMinSeconds != nIntervalMaxSeconds)
         nTimerSeconds += rand() % (abs(static_cast<int>(nIntervalMaxSeconds - nIntervalMinSeconds)) + 1);

      nTimerID = SetTimer(_hSelf, nTimerID, nTimerSeconds * 1000, NULL);

      SYSTEMTIME nextTime{};
      GetSystemTime(&nextTime);
      Utils::addSecondsToTime(nextTime, nTimerSeconds);
      SetDlgItemText(_hSelf, IDC_STAYAWAKE_NEXT_TOGGLE, Utils::formatSystemTime(nextTime, L"Next StayAwake event").c_str());
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
