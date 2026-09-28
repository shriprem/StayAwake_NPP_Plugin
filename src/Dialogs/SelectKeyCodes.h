#pragma once

#include "../Utils.h"
#include "../NPP/StaticDialog.h"
#include "StayAwakePanel.h"

class SelectKeyCodes : public StaticDialog {
public:
   SelectKeyCodes() : StaticDialog() {};
   INT_PTR doDialog(HINSTANCE hInst, StayAwakeCore& awakeCore);

private:
   StayAwakeCore mAwakeCore{};
   INT_PTR CALLBACK run_dlgProc(UINT Message, WPARAM wParam, LPARAM lParam);

   void initPanel();
   void checkAllBoxes(const wstring& sSelectedKeyCodes, int start = IDC_KEY_SCROLL_LOCK, int endNext = IDC_KEY_SCROLL_LOCK + LEN_ROSTER_KEYCODES);
   bool onApply();
   void onClickedMouseMove();
};
