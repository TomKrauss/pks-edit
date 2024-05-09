# Configuration

The basic PKS Edit configuration of PKS Edit is stored in a file named `pkseditini.json` located in the `pks_sys` directory.
Note, that all PKS Edit configuration files are placed in the `pks_sys` sub-folder of the PKS Edit installation folder.
You may set the overall PKS Edit options by pressing the COG icon in the toolbar or via menu "Extra->Settings".

## Configuration Files of PKS Edit

The following files are loaded by PKS Edit during startup form the 'pks_sys' folder.

|File|Purpose|
|----|----|
|`pkseditini.json`|The main editor configuration containing the information edited in the Setup... menu|
|`pkssession.json`|Contains _session_ information such as list of recent searches and open files and currently open editor windows|
|`pkseditconfig.json`|Contains all defined document types (matched by their file name patterns) and the corresponding properties such as associated grammar|
|`pksactionbinding.json`|Contains action binding definitions (keyboard, mouse, menu, context menu) and the associated commands / macros to execute|
|`mylang.grammar.json`|Contains the grammar rules (syntax highlighting, formatting, code completion etc...) for one grammar representing a code language|
|`pksedit.mac`|Contains a binary / compiled version of macros defined. Note, that one may refer to a macro by name from this file e.g. in a save action or an action binding|


## Autosave Options

In this section you may configure all actions performed during startup and shutdown and the way files are automatically saved.

PKS Edit supports two _autosave_ modes: it may either save the files directly automatically or it may save them to a temprary directory. 

- `Autosave time` - can be used to configure the time period during which all changed editors are automatically saved. Setting the time to 0 will disable autosave.
- `Automatically delete autosave temporary files` - if this option is set, PKS Edit will automatically erase all auto-saved files saved to a temporary directory even after crashes when exiting.
- `Autosave path` - configures the PKS Edit temporary directory, where it will autosave files.
- `Save settings on exit` - will have PKS Edit save the options configured during exit
- `Reopen previously open file` - will have PKS Edit maintain the current _editing session_. When PKS Edit exits, it will save the information about all currently open files and windows (in a file named `pksedit.his`) and will
  restore these files, when it is restarted.
- `Save macros on exit` - when checked all recorded or compiled [macros](macro_language.md) are saved automatically to the compiled macro file (`pksedit.mac`).
- `Place backup files in temporary directory` - if this option is set, a copy of a modified file is written to the temporary PKS Edit directory.
- `Save/restore named clipboards` - if this option is set, all text copied to a named clipboard will be made persistent in the temporary directory and restored when PKS Edit starts so named clipboards are preserved through editing sessions.
- `Provide clipboard history` - will maintain a list of recently used clipboard contents automatically.
- `Autosave files to temporary directory or file` - when set, copies of files are saved during autosave (to a temporary directory), when unset, changed files are automatically saved directly.

## Warnings Options

PKS Edit differentiates between major errors (disk full or the like) and minor errors / hints (search pattern not found). Major errors are always shown in modal message boxes. The way minor errors are shown can be configured
in this section.

- `Play sound on error` - when set, a warning sound is played, when an error occurs in PKS Edit.
- `Error flash` - when set, the editor window is _flashed_ during errors.
- `Error window` - when set, a _toast window_ is opened when an error occurs. Otherwise minor errors are shown less intrusive in the status bar.

## Layout Options

Layout Options configure the general UI of PKS Edit

- `Show Optionbar` - when set, the option bar at the bottom of the screen is displayed.
- `Show Functionkeybar` - when set, the function keys are displayed.
- `Show Statusline` - when set, a status line is displayed.
- `Show Toolbar` - when set, the toolbar is displayed.
- `Display Editor Tabs in compact mode` - when set, only the file name is displayed for the tabs of the editors. Note, that one can still see the full filename of a file edited by hovering over the tab or in the window lists.
- `Toolbar Icon Size` - can be used to configure the size of the toolbar icons using 4 predefined sizes.

## Miscellaneous Options

- `Undo available` - should be set by default to allow for undo and redo of actions.
- `Number of undo steps` - configures the maximum number of actions which can be undone. Setting this value to high may cause memory problems, so change with care.
- `Silently reload files changed externally` - if set, externally changed files are silently reloaded (unless modified) by PKS Edit without popping up 
   a confirmation dialog.
- `Hide selection on caret movement` - when the caret is moved and text is selected, the selection "goes away", if this option is set.
- `Open dialogs close to mouse pointer` - when opening a dialog (e.g. Find or Replace), the dialog opens close to the current mouse position, when this option is set.
- `Lock edited files` - when this option is set, files are locked, when they are edited, so they cannot be changed by other applications concurrently.
- `Theme` - allows to select one of the pre-defined editor themes defining fonts and colors used in PKS Edit. The available themes are configured in a file named `themeconfig.json`. 
- `Language` - allows to select the user interface language of PKS Edit (currently only German and English are available - the manual is only available in English).
- `Default Font` - can be used to configure the default text font face used in editing windows, when no explicit font face name is configured in a 
   style defined in the theme configuration.

## Configuration of bindings for keyboard, mouse, context menu 

All Action bindings in PKS Edit are defined in the file `pks_sys\pksactionbindings.json`. This will define all user interface elements triggering
commands in PKS Edit and their binding to a command:

- keyboard shortcuts
- mouse click events
- menubar with menus and items
- context menu
- toolbar.

The action bindings can be extended as all default configuration files in PKS Edit by defining a corresponding _personal configuration file_ - in this
case named %home%\pksedit\config\pksactionbindings.json. Note, that PKS Edit currently defines 3 action contexts which can be associated to a binding
to have the binding only be available in a certain context.

- `default` - the standard context - active by default and contains all actions always available
- `search-list` - active, when viewing / editing a search result list (result of search in files or a compiler output file)
- `markdown-renderer`- active when viewing / navigating a markdown document in wysiwyg mode.

In order to activate a binding only in the given context, specify `"context": "search-list"` (for instance.)

In the following sections, we describe the JSON syntax, which can be used to define action bindings.

### Tips for editing an action binding file
- To generate the list of current bindings use the menu Macro->View Bindings->Keyboard/Mouse/...
- In an action bindings file, you may change the bindings by using code completion to change the desired command (and icon name) 
and you may use Macro->Insert Keycode to insert a keyboard keycode into the file.

### Binding keyboard shortcuts

The following example binds the key `END` to moving the caret to the end of the line and binds the pressing the Space Bar to opening the 
error list match under the caret - but only in action context `search-list`. 

```json
  "key-bindings": [
    {
      "key": "END",
      "command": "@cursor-eol"
    },
    {
      "key": "Ctrl+SPACE",
      "context": "search-list",
      "command": "@errorlist-current-line"
    },...
``` 

Key codes are described as a combination of `Ctrl`, `Shift`, `Win`, `Alt`, `Selected` and "+" and the name of the corresponding key 
(`SPACE`, `ENTER`, 1, 2, 3, A, B, C, ...). Key names are the standard WIN32 virtual key code names with the prefix "VK_" removed (see WinUser.h).

Use for example `Ctrl+Shift+Selected+TAB` to bind the key combination of pressing the Control key, the Shift and the Tab key, when a selection
in the text exists to an action.

Key bindings support the following properties:

- `key` the keycode combination bound
- `command` the command to execute (either a built-in command or the name of a macro)
- `context` the optional action context in which the binding is active.

### Binding mouse events

The following example binds a single right mouse button click to opening the context menu and pressing Shfit + a single right mouse button to mark
one line at the same time displaying an info message.

```json
  "mouse-bindings": [
    {
      "mouse": "MRight(1)",
      "command": "@open-context-menu"
    },
    {
      "mouse": "Shift+MRight(1)",
      "command": "@mouse-mark-line",
      "message": "Press Alt+Shift+MLeft to umark the line"
    },...
``` 

Mouse bindings support the following properties:

- `key` the mouse event description (modifier combination + MLeft|MRight|MMiddle (number of clicks)) to bind
- `command` the command to execute (either a built-in command or the name of a macro)
- `context` the optional action context in which the binding is active.
- `message` an optional message to display
- `message-id` internal integer value for referring to a PKS Edit string resource.

### Context menu and menu bar bindings

Using `context-menu` one can define the context menu in editors in PKS Edit and using `menu` one can define the menubar + main menus and their
respective bindings. The following example defines a context menu with one menu item allowing to select the complete text and a cascading sub-menu
with a label of "Find" and two sub-menu items to find a string and to replace a string. Note, that labels for menus and items can be defined in 2-3 ways:

- by specifying an explicit `label` attribute, which defines the label
- by specifying a `label-id` which is interpreted as an integer value into PKS Edits internal string table
- for menu items bound to commands, PKS Edit may derive an automatic label for the corresponding command (see example `@find-string` and `@replace-string`)

```json
  "context-menu": [
    {
	  "label": "Select all",
      "command": "@select-all"
    },
	{ "label": "Find",
	  "sub-menu": [
        { "command": "@find-string" },
        { "command": "@replace-string" }
	  ]
	}

```

Context menu and menu definitions support the following properties:

- `command` the command to execute (either a built-in command or the name of a macro)
- `context` the optional action context in which the binding is active
- `label` an optional label to display in the item
- `label-id` internal integer value for referring to a PKS Edit string resource to define the label
- `sub-menu` array of menu definitions to be used as a sub-menu
- `separator` may be set to `true` to define a separator item in a menu
- `history` may be set to true to mark the position, where PKS Edit will insert menu items allowing to access a recently used file.
- `anchor` string defining an anchor allowing to modify default menus in extension files
- `insert-at` string value referring to an anchor defined using `id` to be used when extending menus. The corresponding item will be added to
  the menu specified using the anchor name.

### Toolbar definition and bindings

Using `toolbar` one can define the toolbar buttons of PKS Edit. The following example defines a toolbar with two buttons allowing to select 
the complete text a button replace a string. The icons are named as the corresponding AwesomeFont Version 5 icon (PKS Edit uses Awesome Font icons
internally wherever appropriate). Toolbar labels can be defined similar to menu item labels, but are used as tooltips in the toolbar.

```json
  "toolbar": [
    {
	  "icon": "fa-heart",
      "command": "@select-all"
    },
	{ "icon": "fa-search",
	  "command": "@find-string" 
	},
	...
```

Toolbar button definitions support the following properties:

- `command` the command to execute (either a built-in command or the name of a macro)
- `context` the optional action context in which the binding is active
- `label` an optional label (tooltip) to display in the button
- `label-id` internal integer value for referring to a PKS Edit string resource to define the label  (tooltip) 
- `separator` may be set to `true` to define a separator button in the toolbar
- `icon` name of the Awesome Font icon to use
- `anchor` string defining an anchor allowing to modify default toolbar in extension files
- `insert-at` string value referring to an anchor defined using `id` to be used when extending toolbars. The corresponding button will be added after
  the button specified using the anchor name.

## Advanced configuration

Some configuration options can only be configured in the `pkseditini.json` file directly.

Put a property named `sound-name` into the configuration section to configure the error sound played upon errors. 
The following sample selects a high beep tone as the error sound. 

```json
    "sound-name": "HighBeep", 
...
```

The following sound names may be used:

- `Beep` plays a normal beep sound
- `LowBeep` plays a low tone beep sound
- `HighBeep` plays a high tone beep sound
- `Asterisk` plays the windows "Asterisk" sound.
- `Exclamation` plays the windows "Exclamation" sound.
- `Question` plays the windows "Question" sound.

