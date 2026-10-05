#include "SelectKeyCodes.h"
#include "../Utils.h"

static_assert(IDC_MOUSE_MOVE_ZERO - IDC_KEY_SCROLL_LOCK + 1 == LEN_ROSTER_KEYCODES, "Roster checkbox IDs must be consecutive");

extern NppData nppData;
extern StayAwakePanel _awakePanel;


INT_PTR SelectKeyCodes::doDialog(HINSTANCE hInst, StayAwakeCore& awakeCore) {
   Window::init(hInst, nppData._nppHandle);
   mAwakeCore = awakeCore;

   // Blocks until EndDialog() is called from run_dlgProc.
   INT_PTR result = DialogBoxParam(_hInst, MAKEINTRESOURCE(IDD_SELECT_KEYCODES_DIALOG), _hParent, dlgProc, reinterpret_cast<LPARAM>(this));

   // The window has been destroyed by now. Clear the stale handle so
   // isCreated() is false and ~StaticDialog() doesn't try to destroy it again.
   _hSelf = NULL;

   return result;
}

INT_PTR CALLBACK SelectKeyCodes::run_dlgProc(UINT message, WPARAM wParam, LPARAM /*lParam*/) {
   switch (message) {
   case WM_INITDIALOG:
      NppMessage(NPPM_DARKMODESUBCLASSANDTHEME, static_cast<WPARAM>(NppDarkMode::dmfInit), reinterpret_cast<LPARAM>(_hSelf));
      initDialog();
      break;

   case WM_COMMAND:
      switch LOWORD(wParam) {

      case IDC_KEY_SELECT_ALL_BTN:
         checkAllBoxes(wstring(LEN_ROSTER_KEYCODES, L'1'));
         break;

      case IDC_KEY_SELECT_NONE_BTN:
         checkAllBoxes(wstring(LEN_ROSTER_KEYCODES, L'0'));
         break;

      case IDC_KEY_SELECT_ALL_UNASSGND_BTN:
         checkAllBoxes(wstring(LEN_ROSTER_KEYCODES, L'1'), IDC_KEY_UNASSIGNED_1, IDC_KEY_F13);
         break;

      case IDC_KEY_SELECT_ALL_EXT_FN_BTN:
         checkAllBoxes(wstring(LEN_ROSTER_KEYCODES, L'1'), IDC_KEY_F13, IDC_MOUSE_MOVE);
         break;

      case IDC_INPUT_OPTIONS_INFO_BTN:
         ShellExecute(_hSelf, L"open",
            L"https://github.com/shriprem/StayAwake_NPP_Plugin/blob/Version2.0/InputOptions.md", nullptr, nullptr, SW_SHOW);
         break;

      case IDOK:
         if (onApply()) {
            EndDialog(_hSelf, LOWORD(wParam));
            return TRUE;
         }
         break;

      case IDCANCEL:
         EndDialog(_hSelf, LOWORD(wParam));
         return TRUE;
      }
      break;

   }

   return FALSE;
}

void SelectKeyCodes::initDialog() {
   checkAllBoxes(mAwakeCore.GetSelectedKeyCodes());

   Utils::addTooltip(_hSelf, IDC_INPUT_OPTIONS_INFO_BTN, L"", L"View Readme Online", 3, TRUE);
   Utils::loadBitmap(_hSelf, IDC_INPUT_OPTIONS_INFO_BTN, IDB_INFO_BITMAP);

   goToCenter();
}

void SelectKeyCodes::checkAllBoxes(const wstring& sSelectedKeyCodes, int start, int endNext) {
   for (int i{ start }; i < endNext; i++)
      CheckDlgButton(_hSelf, i, sSelectedKeyCodes.at(i % IDC_KEY_SCROLL_LOCK) == L'1');
}

bool SelectKeyCodes::onApply() {
   wstring sSelectedKeyCodes{};

   for (int i{}; i < LEN_ROSTER_KEYCODES; i++)
      sSelectedKeyCodes += (IsDlgButtonChecked(_hSelf, IDC_KEY_SCROLL_LOCK + i)) ? L"1" : L"0";

   if (sSelectedKeyCodes == wstring(LEN_ROSTER_KEYCODES, L'0')) {
      MessageBox(_hSelf, L"Please select at least one Key Code", L"Select multiple Key Codes", MB_ICONEXCLAMATION);
      return false;
   }

   if (!mAwakeCore.SaveSelectedKeyCodes(sSelectedKeyCodes)) {
      MessageBox(_hSelf, L"Unable to save to the StayAwake.ini file.", L"Select multiple Key Codes", MB_ICONEXCLAMATION);
      return false;
   }

   return true;
}

