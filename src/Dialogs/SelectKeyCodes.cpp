#include "SelectKeyCodes.h"

static_assert(IDC_KEY_UNASSIGNED_10 - IDC_KEY_SCROLL_LOCK + 1 == LEN_ROSTER_KEYCODES, "Roster checkbox IDs must be consecutive");

extern NppData nppData;
extern StayAwakePanel _awakePanel;


INT_PTR SelectKeyCodes::doDialog(HINSTANCE hInst) {
   Window::init(hInst, nppData._nppHandle);

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
      goToCenter();
      checkAllBoxes(_awakePanel.getSelectedKeyCodes());
      break;

   case WM_COMMAND:
      switch LOWORD(wParam) {
      case IDC_KEY_SELECT_ALL_BTN:
         checkAllBoxes(wstring(LEN_ROSTER_KEYCODES, L'1'));
         break;

      case IDC_KEY_SELECT_NONE_BTN:
         checkAllBoxes(wstring(LEN_ROSTER_KEYCODES, L'0'));
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

void SelectKeyCodes::checkAllBoxes(const wstring& sSelectedKeyCodes) {
   for (int i{}; i < LEN_ROSTER_KEYCODES; i++)
      CheckDlgButton(_hSelf, IDC_KEY_SCROLL_LOCK + i, sSelectedKeyCodes.at(i) == L'1');
}

bool SelectKeyCodes::onApply() {
   wstring sSelectedKeyCodes{};

   for (int i{}; i < LEN_ROSTER_KEYCODES; i++)
      sSelectedKeyCodes += (IsDlgButtonChecked(_hSelf, IDC_KEY_SCROLL_LOCK + i)) ? L"1" : L"0";

   if (sSelectedKeyCodes == wstring(LEN_ROSTER_KEYCODES, L'0')) {
      MessageBox(_hSelf, L"Please select at least one Key Code", L"Select multiple Key Codes", MB_ICONEXCLAMATION);
      return false;
   }

   if (!_awakePanel.saveSelectedKeyCodes(sSelectedKeyCodes)) {
      MessageBox(_hSelf, L"Unable to save to the StayAwake.ini file.", L"Select multiple Key Codes", MB_ICONEXCLAMATION);
      return false;
   }

   return true;
}
