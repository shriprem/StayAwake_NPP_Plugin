## Version History

### Version 1.1.0.0 [September 22, 2025]

#### Enhancements
1. StayAwake now offers a choice from **12 keycode simulations** to better serve individual user situations.

2. Added **Pause/Resume** button to pause or resume the timer-based key simulations.

3. Implemented **Stealth Mode**. The Stealth Mode will allow users to benefit from StayAwake's core utility without requiring to have its plugin panel open in Notepad++. The plugin panel will only be needed to specify preferences for the plugin.

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

