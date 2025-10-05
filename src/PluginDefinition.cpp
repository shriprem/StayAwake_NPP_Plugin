//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


#include "PluginDefinition.h"
#include "Dialogs/StayAwakePanel.h"

#ifdef UNICODE
   #define generic_itoa _itow
#else
   #define generic_itoa itoa
#endif

FuncItem pluginMenuItems[MI_COUNT];
tTbData  _dockpanelData{};

NppData nppData;
HINSTANCE _gModule;
StayAwakePanel _awakePanel;

bool isStealthMode{};

void pluginInit(HANDLE hModule) {
   _gModule = (HINSTANCE)hModule;
   _awakePanel.init(_gModule, NULL);
}

void pluginCleanUp(){}

void commandMenuInit() {
   setCommand(MI_STAY_AWAKE_PANEL, MENU_SHOW_PANEL, ToggleStayAwakePanel, NULL, _awakePanel.isVisible());
   setCommand(MI_STAY_AWAKE_STEALTH, MENU_STEALTH_MODE, StayAwakeStealthMode, NULL, true);
   setCommand(MI_ABOUT_DIALOG, MENU_ABOUT, ShowAboutDialog);

   StayAwakeStealthMode();
}


void commandMenuCleanUp() {
   delete pluginMenuItems[MI_STAY_AWAKE_PANEL]._pShKey;
}

// Initialize plugin commands
bool setCommand(size_t index, const wstring& cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk, bool checkOnInit) {
    if (index >= MI_COUNT) return false;

    if (!pFunc) return false;

    lstrcpy(pluginMenuItems[index]._itemName, cmdName.c_str());
    pluginMenuItems[index]._pFunc = pFunc;
    pluginMenuItems[index]._pShKey = sk;
    pluginMenuItems[index]._init2Check = checkOnInit;

    return true;
}

LRESULT NppMessage(UINT messageID, WPARAM wparam, LPARAM lparam) {
   return SendMessage(nppData._nppHandle, messageID, wparam, lparam);
}

void RegisterDockPanelIcon() {
   const bool bStandardIcons{ NppMessage(NPPM_GETTOOLBARICONSETCHOICE, 0, 0) == 4 };

   const UINT iconID = (NppMessage(NPPM_ISDARKMODEENABLED, 0, 0)) ?
      (bStandardIcons ? IDI_STAYAWAKE_BTN_STD : IDI_DOCK_DARK_MODE_ICON) :
      (bStandardIcons ? IDI_STAYAWAKE_BTN_STD : IDI_DOCK_LITE_MODE_ICON);

   _dockpanelData.hIconTab = static_cast<HICON>(LoadImage(_gModule, MAKEINTRESOURCE(iconID),
      IMAGE_ICON, 14, 14, LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT));

   NppMessage(NPPM_DMMREGASDCKDLG, 0, reinterpret_cast<LPARAM>(&_dockpanelData));
}

void InitStayAwakePanel() {
   _awakePanel.setParent(nppData._nppHandle);

   if (!_awakePanel.isCreated()) {
      _awakePanel.create(&_dockpanelData);

      _dockpanelData.uMask = DWS_DF_CONT_RIGHT | DWS_ICONTAB;
      _dockpanelData.pszModuleName = _awakePanel.getPluginFileName();
      _dockpanelData.dlgID = MI_STAY_AWAKE_PANEL;
      _dockpanelData.pszName = MENU_PANEL_NAME;
   }
}

// Dockable StayAwake Dialog
void ToggleStayAwakePanel() {
   ShowStayAwakePanel(!_awakePanel.isVisible());
}

void ShowStayAwakePanel(bool show) {
   if (show && !_awakePanel.isPanelInitialized()) {
      InitStayAwakePanel();
      RegisterDockPanelIcon();
      _awakePanel.initPanel();
   }

   _awakePanel.display(show);
}

void StayAwakeStealthMode() {
   if (!isStealthMode) {
      InitStayAwakePanel();
      _awakePanel.initConfig();
   }

   isStealthMode = !isStealthMode;
   Utils::checkMenuItem(MI_STAY_AWAKE_STEALTH, isStealthMode);
   _awakePanel.stealthMode(isStealthMode);
}

void ShowAboutDialog() {
   _awakePanel.showAboutDialog();
}
