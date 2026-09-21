#include "AboutDialog.h"

void AboutDialog::doDialog(HINSTANCE hInst) {
   Window::init(hInst, nppData._nppHandle);

   // Blocks until EndDialog() is called from run_dlgProc.
   DialogBoxParam(_hInst, MAKEINTRESOURCE(IDD_ABOUT_DIALOG), _hParent, dlgProc, reinterpret_cast<LPARAM>(this));

   // The window has been destroyed by now. Clear the stale handle so
   // isCreated() is false and ~StaticDialog() doesn't try to destroy it again.
   _hSelf = NULL;
}

void AboutDialog::localize() {
#ifdef _WIN64
   wstring buildBit{ L" (64-bit)" };
#else
   wstring buildBit{ L" (32-bit)" };
#endif // _WIN64

   SetWindowText(_hSelf, ABOUT_DIALOG_TITLE);

   SetDlgItemText(_hSelf, IDC_ABOUT_NAME, getVersionInfo(L"FileDescription").c_str());

   SetDlgItemText(_hSelf, IDC_ABOUT_VERSION,
      (L"Version: " + getVersionInfo(L"FileVersion") + buildBit).c_str());

   SetDlgItemTextA(_hSelf, IDC_ABOUT_BUILD_TIME,
      ("Build time: " + string(__DATE__) + " - " + string(__TIME__)).c_str());

   SetDlgItemText(_hSelf, IDC_ABOUT_ATTRIBUTION, getVersionInfo(L"LegalCopyright").c_str());
   SetDlgItemText(_hSelf, IDOK, ABOUT_BTN_LABEL_OK);
}

INT_PTR CALLBACK AboutDialog::run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam) {
   switch (message) {
   case WM_INITDIALOG:
      NppMessage(NPPM_DARKMODESUBCLASSANDTHEME, static_cast<WPARAM>(NppDarkMode::dmfInit), reinterpret_cast<LPARAM>(_hSelf));
      localize();
      goToCenter();
      break;

   case WM_COMMAND:
      switch LOWORD(wParam) {
      case IDCANCEL:
      case IDOK:
         EndDialog(_hSelf, LOWORD(wParam));
         return TRUE;
      }
      break;

   case WM_NOTIFY:
      switch (((LPNMHDR)lParam)->code) {
      case NM_CLICK:
      case NM_RETURN:
         ShellExecute(_hSelf, L"open", getVersionInfo(L"CompanyName").c_str(), NULL, NULL, SW_SHOW);
         EndDialog(_hSelf, IDOK);
         return TRUE;
      }
      break;

   }

   return FALSE;
}
