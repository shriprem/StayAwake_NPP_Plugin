# StayAwake plugin for Notepad++

![GitHub](https://img.shields.io/github/license/shriprem/StayAwake_NPP_Plugin)
![GitHub release (latest by date)](https://img.shields.io/github/v/release/shriprem/StayAwake_NPP_Plugin)
![GitHub all releases](https://img.shields.io/github/downloads/shriprem/StayAwake_NPP_Plugin/total)
 &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
![GitHub last commit (by committer)](https://img.shields.io/github/last-commit/shriprem/StayAwake_NPP_Plugin)
![GitHub Workflow Status (with event)](https://img.shields.io/github/actions/workflow/status/shriprem/StayAwake_NPP_Plugin/CI_build.yml)
![GitHub issues](https://img.shields.io/github/issues/shriprem/StayAwake_NPP_Plugin)

[Current Version: 1.0.0.1](https://github.com/shriprem/StayAwake_NPP_Plugin/blob/main/VersionHistory.md)

StayAwake is a simple plugin for Notepad++ that enables you to maintain an _Active_ status on Microsoft Teams (and perhaps other such messaging applications). StayAwake also prevents screen saver activation, screen blanking, and Windows session lockouts.

If you are using Microsoft Teams, you may have noticed that Teams automatically sets your status to _Away_ after a certain period of inactivity. This can be frustrating if you are still at your desk but perhaps on a business phone call _OR_ outlining a plan on paper _OR_ consulting some reference material.

StayAwake is a lightweight plugin for Notepad++ that will continue to run in the background until you click the _Close_ button in its plugin panel.

StayAwake does its job by periodically cycling  the _Scroll Lock_ button (_i.e._, toggling once, and then immediately toggling again to restore _Scroll Lock_ back to its original state). Since the _Scroll Lock_ button is not used by most applications or users, this is a safe and non-intrusive way to keep your Microsoft Teams status _Active_.

## Plugin Panel
![StayAwake UI](https://github.com/shriprem/StayAwake_NPP_Plugin/blob/main/images/StayAwakePanel.png)

**Seconds between each toggle** field:
This field allows you to specify the interval between each _Scroll Lock_ toggle. The initial default value for this is 240 seconds (_i.e._, 4 minutes). However, you can change this to any value between 10 and 9990 seconds. Your new value will be saved and used the next time you run the plugin.

**Set Timer** button: After changing the value for *Seconds between each toggle*, click this button to apply the new setting. Clicking this button will also immediately cycle the _Scroll Lock_ button once.

The time when the app last toggled _Scroll Lock_, and the time when it will do so next are displayed and continually refreshed on the plugin panel.

**Close** button: Click this button to close the plugin panel.


## Installation

### Installation
Within Notepad++, you can install the StayAwake plugin using the Notepad++ Plugin Manager (**Plugins » Plugins Admin** menu option => **Available** tab).

After installing the StayAwake plugin, click on **Plugins » StayAwake » Show StayAwake Panel** menu option to start the utility.


## See Also
StayAwake is also available as a [standalone application](https://github.com/shriprem/StayAwake). This version can be useful if you are not a Notepad++ user.

<a href='https://ko-fi.com/S6S417WICS' target='_blank'><img height='36' style='border:0px;height:36px;' src='https://storage.ko-fi.com/cdn/kofi5.png?v=6' border='0' alt='Buy Me a Coffee at ko-fi.com' /></a>
