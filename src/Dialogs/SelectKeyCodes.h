#pragma once

#include "../Utils.h"
#include "../NPP/StaticDialog.h"
#include "StayAwakePanel.h"

class SelectKeyCodes : public StaticDialog {
public:
   SelectKeyCodes() : StaticDialog() {};
   INT_PTR doDialog(HINSTANCE hInst);

private:
   INT_PTR CALLBACK run_dlgProc(UINT Message, WPARAM wParam, LPARAM lParam);

   void checkAllBoxes(const wstring& sSelectedKeyCodes);
   bool onApply();
};
