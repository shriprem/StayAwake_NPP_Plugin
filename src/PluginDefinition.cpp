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

NppData nppData;
HINSTANCE _gModule;
StayAwakePanel _awakePanel;

void pluginInit(HANDLE hModule) {
   _gModule = (HINSTANCE)hModule;
   _awakePanel.init(_gModule, NULL);
}

void pluginCleanUp(){}

void commandMenuInit() {
   setCommand(MI_STAY_AWAKE_PANEL, MENU_SHOW_PANEL, ToggleStayAwakePanel, NULL, _awakePanel.isVisible());
   setCommand(MI_ABOUT_DIALOG, MENU_ABOUT, ShowAboutDialog);
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

LRESULT nppMessage(UINT messageID, WPARAM wparam, LPARAM lparam) {
   return SendMessage(nppData._nppHandle, messageID, wparam, lparam);
}

UINT getDockPanelIcon() {
   bool bStandardIcons{ nppMessage(NPPM_GETTOOLBARICONSETCHOICE, 0, 0) == 4 };

   if (nppMessage(NPPM_ISDARKMODEENABLED, 0, 0))
      return bStandardIcons ? IDI_STAYAWAKE_BTN_STD : IDI_DOCK_DARK_MODE_ICON;
   else
      return bStandardIcons ? IDI_STAYAWAKE_BTN_STD : IDI_DOCK_LITE_MODE_ICON;
}

// Dockable StayAwake Dialog
void ToggleStayAwakePanel() {
   ShowStayAwakePanel(!_awakePanel.isVisible());
}

void ShowStayAwakePanel(bool show) {
   if (show && !_awakePanel.isVisible()) {
      _awakePanel.setParent(nppData._nppHandle);
      tTbData  data {};

      if (!_awakePanel.isCreated()) {
         _awakePanel.create(&data);

         data.uMask = DWS_DF_CONT_RIGHT | DWS_ICONTAB;
         data.pszModuleName = _awakePanel.getPluginFileName();
         data.dlgID = MI_STAY_AWAKE_PANEL;
         data.pszName = MENU_PANEL_NAME;
         data.hIconTab = (HICON)::LoadImage(_gModule,
            MAKEINTRESOURCE(getDockPanelIcon()), IMAGE_ICON, 14, 14, LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT);

         nppMessage(NPPM_DMMREGASDCKDLG, 0, (LPARAM)& data);

         _awakePanel.initPanel();
      }
   }

   _awakePanel.display(show);
}

void ShowAboutDialog() {
   _awakePanel.showAboutDialog();
}

void refreshDarkMode() {
   if (_awakePanel.isCreated())
      _awakePanel.refreshDarkMode();
}
