## Version History

### Version 1.2.0.1 [September 21, 2026]

1. StayAwake will use a random pick from a roster of user-specified key codes for each Awake event. See enhancement request: [#15](https://github.com/shriprem/StayAwake/issues/15).

2. StayAwake will use random interval between minimum & maximum seconds for each Awake event. See enhancement request: [#15](https://github.com/shriprem/StayAwake/issues/15).

#### Notes for users upgrading from previous versions of StayAwake
1. The user's choice for Key Simulation in the previous versions will be used as the only enabled key code in this new version. Users will need to use the [Select multiple Key Codes](https://github.com/shriprem/StayAwake_NPP_Plugin?tab=readme-ov-file#select-multiple-key-codes) popup to specify multiple key codes to enable Awakes with random key simulations.

2. The user-specified value for the *Seconds between Awakes* field in the previous versions will be used to fill both [Minimum Awake Seconds](https://github.com/shriprem/StayAwake_NPP_Plugin?tab=readme-ov-file#minimum-awake-seconds) and [Maximum Awake Seconds](https://github.com/shriprem/StayAwake_NPP_Plugin?tab=readme-ov-file#maximum-awake-seconds) fields in this new version. Users will need to specify a different value for these two new fields to enable a random interval between Awake events.

**Release:** [1.2.0.1 Release](https://github.com/shriprem/StayAwake_NPP_Plugin/releases/tag/v1.2.0.1)

---

### Version 1.1.0.0 [September 23, 2025]

#### Enhancements
1. StayAwake now offers a choice from [12 keycode simulations](https://github.com/shriprem/StayAwake_NPP_Plugin?tab=readme-ov-file#key-simulation-options) to better serve individual user situations.

2. Added [Pause/Resume](https://github.com/shriprem/StayAwake_NPP_Plugin#pause-button) button to pause or resume the timer-based key simulations.

3. Implemented [Stealth Mode](https://github.com/shriprem/StayAwake_NPP_Plugin?tab=readme-ov-file#stealth-mode). The Stealth Mode will allow users to benefit from StayAwake's core utility without requiring to have its plugin panel open in Notepad++. The plugin panel will only be needed to specify preferences for the plugin.

#### Fixed Issues
With plugin panel closed during NPP launch, changing NPP preferences for Dark Mode or Toolbar Icon set will crash Notepad++.
Issue [#5](https://github.com/shriprem/StayAwake_NPP_Plugin/issues/5)

**Release:** [1.1.0.0](https://github.com/shriprem/StayAwake_NPP_Plugin/releases/tag/v1.1.0.0)

---

### Version 1.0.1.0 [June 16, 2025]
* Removed custom Darkmode code. Switched to Darkmode rendering provided by Notepad++. This switch will reduce the plugin DLL size by 37KB for the 64-bit version, and by 48KB for the 32-bit version.
* Dock Panel icon will be rendered with Fluent or Standard icon depending on the selection under `Notepad++ menu: Settings » Preferences... » Toolbar`. This feature requires Notepad++ v8.8.2 or later.

**Release:** [1.0.1.0](https://github.com/shriprem/StayAwake_NPP_Plugin/releases/tag/v1.0.1.0)

---

### Version 1.0.0.2 [March 27, 2025]
* Fixed URL linking to the project GitHub page in version info for the plugin DLL, and the hyperlink in the About dialog for the plugin. Issue: [github link incorrect](https://github.com/shriprem/StayAwake_NPP_Plugin/issues/1).

**Release:** [1.0.0.2 Release](https://github.com/shriprem/StayAwake_NPP_Plugin/releases/tag/v1.0.0.2)

---

### Version 1.0.0.1 [March 15, 2025]
* Added range check while reading the **TimerIntervalInSeconds** setting in the application configuration file.

**Release:** [1.0.0.1 Release](https://github.com/shriprem/StayAwake_NPP_Plugin/releases/tag/v1.0.0.1)

---

### Version 1.0.0.0 [December 25, 2024]
Initial Public Release
**Release:** [1.0.0.0 Release](https://github.com/shriprem/StayAwake_NPP_Plugin/releases/tag/v1.0.0.0)

