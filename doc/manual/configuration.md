# Configuration

The basic PKS-Edit configuration of PKS-Edit is stored in a file named `pksedit.ini` located in the `pks_sys` directory.
Note, that all PKS-Edit configuration files are placed in the `pks_sys` sub-folder of the PKS-Edit installation folder.
You may set the overall PKS-Edit options by pressing the COG icon in the toolbar or via menu "Extra->Settings".

## Autosave Options

In this section you may configure all actions performed during startup and shutdown and the way files are automatically saved.

PKS-Edit supports two _autosave_ modes: it may either save the files directly automatically or it may save them to a temprary directory. 

- `Autosave time` - can be used to configure the time period during which all changed editors are automatically saved. Setting the time to 0 will disable autosave.
- `Automatically delete autosave temporary files` - if this option is set, PKS-Edit will automatically erase all auto-saved files saved to a temporary directory even after crashes when exiting.
- `Autosave path` - configures the PKS-Edit temporary directory, where it will autosave files.
- `Save settings on exit` - will have PKS-Edit save the options configured during exit
- `Reopen previously open file` - will have PKS-Edit maintain the current _editing session_. When PKS-Edit exits, it will save the information about all currently open files and windows (in a file named `pksedit.his`) and will
  restore these files, when it is restarted.
- `Save macros on exit` - when checked all recorded or compiled [macros](macro_language.md) are saved automatically to the compiled macro file (`pksedit.mac`).
- `Place backup files in temporary directory` - if this option is set, a copy of a modified file is written to the temporary PKS-EDIT directory.
- `ave/restore named clipboards` - if this option is set, all text copied to a named clipboard will be made persistent in the temporary directory and restored when PKS-Edit starts so named clipboards are preserved through editing sessions.
- `Provide clipboard history` - will maintain a list of recently used clipboard contents automatically.
- `Autosave files to temporary directory or file` - when set, copies of files are saved during autosave (to a temporary directory), when unset, changed files are automatically saved directly.

## Warnings Options

PKS-Edit differentiates between major errors (disk full or the like) and minor errors / hints (search pattern not found). Major errors are always shown in modal message boxes. The way minor errors are shown can be configured
in this section.

- `Play sound on error` - when set, a warning sound is played, when an error occurs in PKS-Edit.
- `Error flash` - when set, the editor window is _flashed_ during errors.
- `Error window` - when set, a _toast window_ is opened when an error occurs. Otherwise minor errors are shown less intrusive in the status bar.

## Layout Options

Layout Options configure the general UI of PKS-Edit

- `Show Optionbar` - when set, the option bar at the bottom of the screen is displayed.
- `Show Functionkeybar` - when set, the function keys are displayed.
- `Show Statusline` - when set, a status line is displayed.
- `Show Toolbar` - when set, the toolbar is displayed.
- `Display Editor Tabs in compact mode` - when set, only the file name is displayed for the tabs of the editors. Note, that one can still see the full filename of a file edited by hovering over the tab or in the window lists.
- `Toolbar Icon Size` - can be used to configure the size of the toolbar icons using 4 predefined sizes.

## Miscellaneous Options

- `Undo available` - should be set by default to allow for undo and redo of actions.
- `Number of undo steps` - configures the maximum number of actions which can be undone. Setting this value to high may cause memory problems, so change with care.
- `Hide selection on caret movement` - when the caret is moved and text is selected, the selection "goes away", if this option is set.
- `Open dialogs close to mouse pointer` - when opening a dialog (e.g. Find or Replace), the dialog opens close to the current mouse position, when this option is set.
- `Lock edited files` - when this option is set, files are locked, when they are edited, so they cannot be changed by other applications concurrently.
- `Theme` - allows to select one of the pre-defined editor themes defining fonts and colors used in PKS-Edit. The available themes are configured in a file named `themeconfig.json`. 
- `Language` - allows to select the user interface language of PKS-Edit (currently only German and English are available - the manual is only available in English).
- `Default Font` - can be used to configure the default text font face used in editing windows, when no explicit font face name is configured in a 
   style defined in the theme configuration.

## Binding (keyboard, mouse, context menus) configuration

To change keyboard, mouse or context menu bindings, please edit the file `pks_sys\pksactionbindings.json` and add / change the corresponding
binding from a key / mouse to the command to execute.

### Tips
- To generate the list of current keyboard bindings use the menu Macro->View Bindings->Keyboard
- In the action bindings file, you may change the bindings by using code completion to change the desired command and you may use Macro->Insert Keycode to
insert a keyboard keycode into the file.

## Advanced configuration

Some configuration options can only be configured in the `pksedit.ini`  file directly.

In the section `[desk]` you may for instance configure the error sound played upon errors. The following sample selects a high beep tone
as the error sound. The following sound names may be used:

- `Beep` plays a normal beep sound
- `LowBeep` plays a low tone beep sound
- `HighBeep` plays a high tone beep sound
- `Asterisk` plays the windows "Asterisk" sound.
- `Exclamation` plays the windows "Exclamation" sound.
- `Question` plays the windows "Question" sound.

```
[desk]
ErrorSound=HighBeep
.....
```
