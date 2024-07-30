# Configuration
The basic PKS Edit configuration of PKS Edit is stored in a file named `pkseditini.json` located in the `pks_sys` directory.
Note, that all PKS Edit configuration files are placed in the `pks_sys` sub-folder of the PKS Edit installation folder.

## User Specific Configuration
One may place config files into a `.pksedit\config` directory located in `$HOME` and extend the installed configuration
files (e.g. `pksactionbindings.json`). This will allow to override or extent key bindings, add new menu entries
etc. 

If you place the same files into `.pksedit\pks_sys`, they will completely override the default files provided in the PKS Edit
installation. Note, that in the later case, you should review overridden files, when installing a new version of PKS Edit.

## Configuration File Search PATH
Configuration files are looked up in the following directories:
- _.\PKS_SYS_
- _%user.home\.pksedit\config_
- _%user.home\.pksedit\PKS_SYS_
- _%appinstalldir%\PKS_SYS_

### Configuring PKS Edit using the UI
You may set the overall PKS Edit options by pressing the COG icon in the toolbar or via menu "Extra->Settings".

## Configuration Files of PKS Edit
The following files are loaded by PKS Edit during startup from the `pks_sys` folders (see above).

|File|Purpose|
|----|----|
|`pkseditini.json`|The main editor configuration containing the information edited in the Setup... menu|
|`pkssession.json`|Contains _session_ information such as list of recent searches and open files and currently open editor windows|
|`pkseditconfig.json`|Contains all defined document types (matched by their file name patterns) and the corresponding properties such as associated grammar|
|`pksactionbinding.json`|Contains action binding definitions (keyboard, mouse, menu, context menu) and the associated commands / macros to execute|
|`mylang.grammar.json`|Contains the grammar rules (syntax highlighting, formatting, code completion etc...) for one grammar representing a code language|
|`pksedit.mac`|Contains a binary / compiled version of macros defined. Note, that one may refer to a macro by name from this file e.g. in a save action or an action binding|
|`copyright_profiles.json`|Defines optional copyright profiles|

## Save Options
In this section you may configure all actions performed during startup and shutdown and the way files are automatically saved.

PKS Edit supports two _autosave_ modes: it may either save the files directly automatically or it may save them to a temprary directory. 

- _Autosave time_ - can be used to configure the time period during which all changed editors are automatically saved. Setting the time to 0 will disable autosave.
- _Automatically delete autosave temporary files_ - if this option is set, PKS Edit will automatically erase all auto-saved files saved to a temporary directory even after crashes when exiting.
- _Autosave path_ - configures the PKS Edit temporary directory, where it will autosave files.
- _Reopen previously open files_ - will have PKS Edit maintain the current _editing session_. When PKS Edit exits, it will save the information about all currently open files and windows (in a file named `pksedit.his`) and will
  restore these files, when it is restarted.
- _Save macros on exit_ - when checked all recorded or compiled [macros](macro_language.md) are saved automatically to the compiled macro file (`pksedit.mac`).
- _Place backup files in temporary directory_ - if this option is set, a copy of a modified file is written to the temporary PKS Edit directory.
- _Save/restore named clipboards_ - if this option is set, all text copied to a named clipboard will be made persistent in the temporary directory and restored when PKS Edit starts so named clipboards are preserved through editing sessions.
- _Provide clipboard history_ - will maintain a list of recently used clipboard contents automatically.
- _Autosave files to temporary directory or file_ - when set, copies of files are saved during autosave (to a temporary directory), when unset, changed files are automatically saved directly.

## Notification Options
PKS Edit differentiates between major errors (disk full or the like) and minor errors / hints (search pattern not found). Major errors are always shown in modal message boxes. The way minor errors are shown can be configured
in this section.

- _Play sound on error_ - when set, a warning sound is played, when an error occurs in PKS Edit.
- _Flash window on error_ - when set, the editor window is _flashed_ during errors.
- _Display error in popup window_ - when set, a _toast window_ is opened when an error occurs. Otherwise minor errors are shown less intrusive in the status bar.

## Layout Options
Layout Options configure the general UI of PKS Edit

- _Show Optionbar_ - when set, the option bar at the bottom of the screen is displayed.
- _Show Functionkeybar_ - when set, the function keys are displayed.
- _Show Statusline_ - when set, a status line is displayed.
- _Show Toolbar_ - when set, the toolbar is displayed.
- _Display Editor Tabs in compact mode_ - when set, only the file name is displayed for the tabs of the editors. Note, that one can still see the full filename of a file edited by hovering over the tab or in the window lists.
- _Toolbar Icon Size_ - can be used to configure the size of the toolbar icons using 4 predefined sizes.

## Miscellaneous Options
- _Undo available_ - should be set by default to allow for undo and redo of actions.
- <a name="number-of-undo-steps">_Number of undo steps_</a> - configures the maximum number of actions which can be undone. Setting this value to an excessive hight value may cause memory problems, so change with care.
- _Maximum number of open windows_ - configures the maximum number of windows to display at a time. If a new window is opened, causing the number of open windows to
  exceed the configured maximum, PKS Edit will automatically close one of the open windows. Setting this value to 0 will disable a limit of open
  windows.
- _Hide selection on caret movement_ - when the caret is moved and text is selected, the selection "goes away", if this option is set.
- _Open dialogs close to mouse pointer_ - when opening a dialog (e.g. Find or Replace), the dialog opens close to the current mouse position, when this option is set.
- _Silently reload files changed externally_ - if set, externally changed files are silently reloaded (unless modified) by PKS Edit without popping up 
   a confirmation dialog.
- _Lock edited files_ - when this option is set, files are locked, when they are edited, so they cannot be changed by other applications concurrently.
- _Theme_ - allows to select one of the pre-defined editor themes defining fonts and colors used in PKS Edit. The available themes are configured in a file named `themeconfig.json`. 
- _Language_ - allows to select the user interface language of PKS Edit (currently only German and English are available - the manual is only available in English).
- _Default Font_ - can be used to configure the default text font face used in editing windows, when no explicit font face name is configured in a 
   style defined in the theme configuration.
- _Reuse running PKS Edit instance_ - if this option is set, opening a file (from the command line or via Windows Explorer) will not
  start a new PKS Edit application if PKS Edit is running already, but will have the running version open that file.
- _Default search engine_ - this is used, when the`_Search on the Internet_` function (Find menu) is used. The expression to find
  will then be delegated to the configured search engine.


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

- _default_ - the standard context - active by default and contains all actions always available
- _search-list_ - active, when viewing / editing a search result list (result of search in files or a compiler output file)
- _markdown-renderer_- active when viewing / navigating a markdown document in wysiwyg mode.

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

- _key_ the keycode combination bound
- _command_ the command to execute (either a built-in command or the name of a macro)
- _context_ the optional action context in which the binding is active.

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

- _key_ the mouse event description (modifier combination + MLeft|MRight|MMiddle (number of clicks)) to bind
- _command_ the command to execute (either a built-in command or the name of a macro)
- _context_ the optional action context in which the binding is active.
- _message_ an optional message to display
- _message-id_ internal integer value for referring to a PKS Edit string resource.

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

- _command_ the command to execute (either a built-in command or the name of a macro)
- _context_ the optional action context in which the binding is active
- _label_ an optional label to display in the item
- _label-id_ internal integer value for referring to a PKS Edit string resource to define the label
- _sub-menu_ array of menu definitions to be used as a sub-menu
- _separator_ may be set to `true` to define a separator item in a menu
- _history_ may be set to true to mark the position, where PKS Edit will insert menu items allowing to access a recently used file.
- _anchor_ string defining an anchor allowing to modify default menus in extension files
- _insert-at_ string value referring to an anchor defined using `id` to be used when extending menus. The corresponding item will be added to
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

- _command_ the command to execute (either a built-in command or the name of a macro)
- _context_ the optional action context in which the binding is active
- _label_ an optional label (tooltip) to display in the button
- _label-id_ internal integer value for referring to a PKS Edit string resource to define the label  (tooltip) 
- _separator_ may be set to `true` to define a separator button in the toolbar
- _icon_ name of the Awesome Font icon to use
- _anchor_ string defining an anchor allowing to modify default toolbar in extension files
- _insert-at_ string value referring to an anchor defined using `id` to be used when extending toolbars. The corresponding button will be added after
  the button specified using the anchor name.

## Advanced configuration
Not all configuration options in PKS Edit can be changed yet in the PKS Edit Configuration Dialog. 
Some configuration options can only be configured in the `pkseditini.json` file directly.

**Important Caveat**: changining these options from within PKS Edit requires, that you do not open
the configuration dialog before closing PKS Edit as closing the configuration dialog will then override
the old options from the last session.

### Configuring Sounds
Put a property named `sound-name` into the configuration section to configure the error sound played upon errors. 
The following sample selects a high beep tone as the error sound. 

```json
    "sound-name": "HighBeep", 
...
```

The following sound names may be used:

- _Beep_ - plays a normal beep sound
- _LowBeep_ - plays a low tone beep sound
- _HighBeep_ - plays a high tone beep sound
- _Asterisk_ - plays the windows "Asterisk" sound.
- _Exclamation_ - plays the windows "Exclamation" sound.
- _Question_ - plays the windows "Question" sound.

### Configuring Build Log Parsing
One can configure formats to be tried, when parsing build output log to let PKS Edit navigate along the errors / warnings
found in the build log using a configuration like in the following sample (comments must of course be ommitted and exist
for explanation purpos):

```json
"compiler-output-patterns": [
  {
    "name": "PKSMAKROC",
    // The pattern to match one navigatable build result
    "pattern": "^[^ ]+ ([^ ]+) ([0-9]+): *(.*)",
    // An optional file name pattern to file name patterns for quick auto-detection of patterns to use
    "filename-pattern": ".pkc",
    // The index of the bracket in pattern to match the file name part
    "filename-capture": 1,
    // The index of the bracket in pattern to match the line number part
    "linenumber-capture": 2,
    // The index of the bracket in pattern to match the comment part
    "comment-capture": 3
  },
  {
    "name": "Java",
    "pattern": "^ *\\[[^]]+\\] *(.+):([0-9]+): error:*(.*)",
    "filename-pattern": ".java",
    "filename-capture": 1,
    "linenumber-capture": 2,
    "comment-capture": 3
  },
```

## Configuring Copyright Profiles
_Copyright Profiles_ allow to define copyrights to be inserted either using a template (press copyright and Ctrl+SPACE to expand in files supporting copyrights) 
or to be inserted, when a new file of type supporting a file template is being created.

Copyrights are configured in a file named `copyright_profiles.json`. One may define different copyright headers and
set options for inserting the copyright headers into a file in a language specific way. Here is a sample defining
one profile named `pks-edit` - setting that as the default profile and defining, that in `Dart` files copyrights are
inserted using single line comments.

In the copyright text you may use [template variables](grammar.md#template-variables).

```json
{
  "default": "pks-edit",
  "profiles": [
	  {
	  	"name": "pks-edit",
			"notice": "${file_name}\n\n${cursor}DESCRIPTION${selection_end}\n\nauthor: ${user}\n\nThis Source Code Form is subject to the terms of the Mozilla Public\nLicense, v. 2.0. If a copy of the MPL was not distributed with this\nfile, You can obtain one at https://mozilla.org/MPL/2.0/.\n\n"
	  }
  ],
  "language-options": [
	  {
			"language": "dart",
			"single-line-comments": true
	  }
  ]
}
```
