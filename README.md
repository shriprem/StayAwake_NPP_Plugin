# StayAwake plugin for Notepad++

![GitHub](https://img.shields.io/github/license/shriprem/StayAwake_NPP_Plugin)
![GitHub release (latest by date)](https://img.shields.io/github/v/release/shriprem/StayAwake_NPP_Plugin)
![GitHub all releases](https://img.shields.io/github/downloads/shriprem/StayAwake_NPP_Plugin/total)
 &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
![GitHub last commit (by committer)](https://img.shields.io/github/last-commit/shriprem/StayAwake_NPP_Plugin)
![GitHub Workflow Status (with event)](https://img.shields.io/github/actions/workflow/status/shriprem/StayAwake_NPP_Plugin/CI_build.yml)
![GitHub issues](https://img.shields.io/github/issues/shriprem/StayAwake_NPP_Plugin)

[Current Version: 1.1.0.0](https://github.com/shriprem/StayAwake_NPP_Plugin/blob/main/VersionHistory.md)

StayAwake is a simple, lightweight plugin for Notepad++ that enables you to maintain an _Active_ status on Microsoft Teams (and perhaps other messaging applications such as Slack and Zoom). StayAwake also prevents screen saver activation, screen blanking, and Windows session lockouts.

If you are using Microsoft Teams, you may have noticed that Teams automatically sets your status to _Away_ after a few minutes of inactivity. This can be frustrating if you are still at your desk but perhaps on a business phone call _OR_ outlining a plan on paper _OR_ consulting some reference material.

StayAwake does its job by simulating key presses of unused or low side-effect buttons at specified time intervals. This is a safe and non-intrusive way to keep your Microsoft Teams status _Active_.

StayAwake will continue to run in the background even after the plugin panel is closed.

## Plugin Panel
![StayAwake UI](images/StayAwakePanel.png)

#### StayAwake Key simulation list
For most users, the default choice of *Scroll Lock cycling* will work just fine. See the [Key Simulation Options](#key-simulation-options) section below for more info.

#### Seconds between Awakes field
This field allows you to specify the interval between key simulations. The initial default value for this is 240 seconds (_i.e._, 4 minutes). However, you can change this to any value between 10 and 9990 seconds. Your new value will be saved and used the next time you run the plugin.

#### Set Timer button
After changing the value for *Seconds between Awakes*, click this button to apply the new setting. Clicking this button will also immediately run a StayAwake Key simulation.

The clock times of when the StayAwake event was last run, and when it will do so next are displayed and continually refreshed on the plugin panel.

#### Pause button
Click this button to pause StayAwake Key simulations.

![StayAwake UI](images/StayAwakePaused.png)

As seen in the clip above, clicking the *Pause* button will also have these effects:
1. The *PAUSED* state will be indicated in the *Next StayAwake event* time slot.
2. The *Pause* button will change to _Resume_ mode. To exit the *PAUSED* state, click the **Resume** button.
3. If Notepad++ is closed while StayAwake is in paused mode, StayAwake will reopen in paused mode when Notepad++ is launched the next time. To exit the *PAUSED* state, click the **Resume** button.

   ![StayAwake UI](images/StayAwakePausedLaunch.png)


#### Close button
Click this button to close the plugin panel and continue to run StayAwake in the background.

### Key Simulation Options

![StayAwake UI](images/StayAwakeKeySelections.png)


#### Scroll Lock cycling
This is the default option, and it will work just fine for most users.

However, some users using Remote Desktop alongside have reported spurious keystrokes being transmitted between the connected Windows devices. Users in such situations should explore other key simulation options.

#### Volume Up & Down
With this option, StayAwake will simulate a _Volume Down_ key press, immediately followed by a _Volume Up_ key press, thereby resulting in no net change in volume level.

A minor benefit with this option is the visual feedback. Each StayAwake key simulation event will cause the Windows Volume Slider bar to display for a few seconds above the Windows Taskbar.

However, if a user has muted the audio, then this option will unmute it. Users wishing to avoid this side effect should explore other key simulation options.

#### Unassigned Key (1 to 10)
With these options, StayWake will simulate key press of an unassigned keycode.

These unassigned keycodes have been obtained from this Microsoft article on [Virtual-Key Codes](https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes).

Microsoft may start utilizing any of these unassigned codes in the future -- but not all of them in one go. Hence all 10 of the currently unassigned codes have been included in the [StayAwake Key Simulation list](#stayawake--key-simulation-list) so that users can switch to another still available unassigned keycode.


### Alternative Tip
If you are only looking for a way to block screen saver activation and session lockouts, you do not actually need the StayAwake plugin. Instead, you can simply open a PDF document in full screen mode in Adobe Acrobat (Reader | Professional), and leave it open in the background. This will prevent your screen saver from activating, and also prevent your Windows session from locking out.

**Note**: Full screen mode is different than maximized window state. To view a PDF document in full screen mode in Adobe Acrobat (Reader | Professional), either use:
* The menu option: *View » Full screen mode*
* Or the much quicker keyboard shortcut: *Ctrl + L*.

However, if you are looking for a way to keep your Microsoft Teams status _Active_, then StayAwake is the right tool for you.

## Installation

Within Notepad++, you can install the StayAwake plugin using the Notepad++ Plugin Manager (**Plugins » Plugins Admin** menu option => **Available** tab).

After installing the StayAwake plugin, click on **Plugins » StayAwake » Show StayAwake Panel** menu option to start the utility.


## See Also
StayAwake is also available as a [standalone application](https://github.com/shriprem/StayAwake). This version can be useful if you are not a Notepad++ user.

<a href='https://ko-fi.com/S6S417WICS' target='_blank'><img height='36' style='border:0px;height:36px;' src='https://storage.ko-fi.com/cdn/kofi5.png?v=6' border='0' alt='Buy Me a Coffee at ko-fi.com' /></a>


