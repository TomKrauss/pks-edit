# Document Types
The document types of PKS Edit are configured in a file named `pkseditconfig.json` located in the `pks_sys` directory.

A document type is selected by one or more file name patterns (e.g. \*.cpp or \*.cpp;\*.c). Note, that _more specific_ file name patterns are matched first (\*.c matches in preference over \*.\*).
Each document type may be associated with an editor configuration (also by name) defining things like the standard display mode or tabstops, [insertion options](editing_files.md#editing-text) and
a _grammar_ which is referred to by name.

## Grammar
A PKS Edit _grammar_ defines the syntax of a piece of text and defines things like the syntax highlighting, 
possible templates, that make sense in the context of this text and rules for editing text with this syntax like 
brackets to be matched etc... A _grammar is defined by a name and configured in corresponding JSON file name `grammarName`.grammar.json.

- Format of [Grammar and Template Files](grammar.md)

## Editor Configuration
The following example defines an editor configuration `default`, which defines some standard tabstops, margins, tabsizes, and options for editing and displaying text.

```json
    "editorConfigurations": [
    {
        "name": "default",
        "id": 1,
        "leftMargin": 0,
        "rightMargin": 100,
        "tabSize": 4,
        "codepage": 65001,
        "tabulatorSizes": [4],
        "shiftWidth": 4,
        "showWhitespace": true,
        "showRuler": true,
        "showLineNumbers": true,
        "showSyntaxHighlight": true,
        "highlightCaretLine": true,
        "insertMode": true,
        "autoIndent": true,
        "codepage": -1,
        "expandAbbreviations": true,
        "smartSpaceDelete": true,
        "tabCharacter": "\t",
        "fillCharacter": " ",
        "newlineCharacter": "\n",
        "crCharacter": "\r",
        "scrollVerticallyBy": 0,
        "scrollHorizontallyBy": 0,
        "scrollVerticalBorder": 5,
        "caretSearchPlacement": 1,
        "scrollSupportTrackThumb": true,
        "caretPreserveColumn": true,
        "caretFollowsScrollbar": true,
        "backupExtension": "bak"
    },

```
 
### Editor Configuration Properties
One can change the editor configuration editing the JSON files or by opening the _Settings->Change Editor Settings..._ menu. Note,
that applying the changes in the Change Editor settings dialog will save the editor configuration to the default configuration file.
Many of the options can also be changed _transiently_ (e.g. by changing them via the option bar or the settings menu), which will not
update the configuration file.

Here is a description of some of the defined properties of an `editor configuration`.


#### <a name="display-options">Display Options</a>
- `showWhitespace` (_Tabs and Spaces_) - if checked / set to true, spaces and tabs are displayed using control characters
- `showSyntaxHighlight`- if set to true, will have the text of documents with a defined grammar being colored using the styling rules defined in the grammar.
- `showLineNumbers` - if set to true, displays the line number area of a editing window. This will (as the name suggests) display the line numbers, but also
  additional things like the change annotations.
- `shiftWidth` the number of columns defining one `logical edited tab` - i.e. the number of columns by which a selected range of text is indented, when pressing Tab.
- `tabSize` - the number of columns defining one `logical visible tab`
- `highlightCaretLine` - if set to true, displays the line in which the input caret is placed using a slightly different background color making 
  the input caret easier to detect.
- `hexMode` - if set to true, the file is edited in hex editing mode.
- `showWysiwyg` (_Wysiwyg Display) - if set to true, the file is displayed in Wysiwyg mode - this is supported only by documents supporting
  a Wysiwyg display (e.g. markdown documents, HTML or images).
- `hideVerticalSlider` (_No vertical slider_) - if checked, no vertical scrollbar is displayed.
- `hideHorizontalSlider` (_No horizontal slider_) - if checked, no horizontal scrollbar is displayed.

#### <a name="editing">Editing</a>
- `codepage` - the standard character encoding / code page for documents of this type. Specify either a WIN32 code for the desired encoding (0 == `ISO8859-1` or 
   65001 == `UTF-8`) or -1 to have PKS Edit "guess" the character encoding.
- `actionContext` - a context to use for documents of the described types for the bound _actions_. Every PKS Edit action binding (keyboard binding, mouse binding
  menus, ...) can have an _action context_ in which the corresponding binding is active. If no action context is defined explicitly a default action context
  is assumed.
- `watchLogfile` (_Log File Mode_) - used for Log Output Files to watch them in the background and automatically reload them
  if that is the case. If this is options is selected, the file is also displayed read-only and cannot be changed.
- `expandAbbreviations` (_Abbreviations_) - if set to true, templates defined in the [grammar](grammar.md) which are marked with `auto-insert=true` will be
  automatically expanded, when a match with the template pattern is encountered during text input.
- `fillCharacter` (Fill Tabs with)- if set to a value != 0 (if entering a character into the corresponding input field in the UI), 
  will have all `TAB` characters being expanded to the assigned character, if `TAB` is pressed or if formatting requires
  to fill in new tabstop position.
- `autoIndent` - whether entering a new line should try to automtically insert the "proper number of tabs" at the beginning of the new line depending
  on the number of tabs displayed in the prvious line etc...
- `autoFormat` (_Format text while typing_) - in particular useful, when editing text files. Will automatically wrap long
  lines _while you type_ and maintain the proper indents etc...
- `tabulatorSizes` - can be assigned to an array of values used as the respective tab stop sizes. If the array size is 1, the single contained 
  element defines the tabstop size.
- `smartSpaceDelete` (_Delete multiple spaces_) - in particular helpful, when working in a mode, where `TAB` characters
  are always expanded to `SPACE` characters. When pressing the `BACKSPACE` key to delete the previous `SPACE` character, PKS
  EDIT checkts, whether there are multiple `SPACE` characters originally created to fill up a `TAB` space and will them
  delete with a single key press, if that is the case.
- `fixCapitals` (_Correct capitals_) - can be set to true, to have PKS Edit try to correct misspelled upper/lower case (e.g. "this is the end. now we start lower case")
- `executeOnLoad` (_Life Cycle Action on Start_) - a PKS Edit macro to execute, when a file of this type is loaded.
- `executeOnSave` (_Life Cycle Action on Save_) - a PKS Edit macro to execute, when a file of this type is saved.

#### <a name="file-format">File Format</a>
- `newlineCharacter` (_Separator 1_) - configures the regular line delimiter. This will be `\n` in most cases. You may however disable
  line separators (e.g. to open and save binary files with no line separator handling).
- `crCharacter` (_Skip before Separator 1_) - configures the existance or the default use of the secondary carriage return line delimiter.
- `newlineCharacter2` (_Separator 2_) - configures an optional secondary line delimiter character for very advanced use cases.
- `backupExtension` (_File Suffix Backup Files_) - configures the extension of backup files to create, when saving files (by default '.bak').
- _Password protected_ - this cannot be configured using JSON, but allows to save a file with password protection. If a password protected
  file is opened, one needs to provide a password to be allowed to open it and view the contents. The contents will be saved then in an encrypted
  way.

#### <a name="caret-and-scrolling">Caret and Scrolling</a>
- `scrollHorizontallyBy` (_Minimum number of columns to scroll..._) - can be used to configure the number of columns to scroll horizontally,
  when reaching the end of the page. Setting this to a value > 1 will cause PKS Edit to jump scoll every time the right maximum is reached.
- `scrollVerticallyBy` (_Minimum number of lines to scroll..._) - can be used to configure the number of lines to scroll vertically,
  when reaching the end of the page. 
- `scrollVerticalBorder` (_Maximum distance of caret...) - Setting this to a value > 1 will have PKS Edit start to scroll already when
  the caret is the configured number of lines away from the top or end of screen, when moving the caret vertically.
- `scrollSupportTrackThumb` (_Update screen during scrolling_) - when this option is set, dragging the scrollbar thumb will at the
  same time update the screen. If this option is not set, the screen will be updated, when the scrollbar thumb is released.
- `caretPreserveColumn` (_Keep screen column..._) - when set, moving the caret vertically will cause PKS Edit to try to position
  in the right most column in which the caret was positioned before.
- `caretSearchPlacement` (_Caret placement after searching_) - allows to configure the position of the cursor on the screen, when the search
  would end up on a place not visible on the screen.

## Example
Here is an example of a document type descriptor for C and C++ files, which associates the files with a grammar named `cpp` and connects the files with a `default` editor configuration.

```json
{
    "documentTypes": [
    {
        "name": "c+cpp",
        "description": "C/ C++ files (*.c,*.h,*.cpp,*.hpp)",
        "grammar": "cpp",
        "languages": ["c", "c++", "cpp"],
        "editorConfiguration": "default",
        "filenamePatterns": "*.c;*.h;*.cpp;*.hpp"
    },
...

```

The following properties describe a document type:
- `name` a descriptive name used as a reference e.g. in the UI of PKS Edit
- `description` documents the purpose of the document type and is also displayed in the file-selector of PKS Edit to help selecting documents
  of the given type.
- `grammar` will be matched with a **grammar file** named `$grammar.grammar.json`, which describes the [grammar](grammar.md) of the documents
  of the given type. Is also used as the primary `language` name of the programming language of the given documents.
- `languages` optional alternate language names. These are used e.g. when matching languages specified in *fenced code blocks*  in mark down
  documents.
- `editorConfiguration` refers to a previously defined editor configuration, which describes tab settings etc...
- `filenamePatterns` used to match a file opened with a document type. For each document type defined the file selector will display an
  entry in the file type selection dropdown list in the file selector.

## Document Types Dialog
One may change the document type (and grammar) of a file by opening the _Change Document Types_ dialog available from
the _Settings->Change Document Type..._ menu. In the dialog which comes up in this case, all known document types
are listed.

Selecting a document type shows the properties of it:

- _Editor Configuration_: this is the name of the defined editor configuration, which defines the default editing flags
  for the selected document type (e.g. Whether whitespace characters should be dispayed etc... - see below for details).
- _Name_: this is the name of the document type.
- _File Name_: this field defines the file name pattern of all files per default associated with this document type. One
  may specify multiple file name patterns separated by `;` as e.g. `*.html;*.xml`.
- _Description_: this is a description of the document type describing the purpose of it.
- _Grammar_: the name of the default _grammar_ associated with this document type.

If you press the <a name="apply>_Apply_</a> button, the document type selected in the document types list
is applied to the current editor window. This allows you to e.g. change the grammar used and to bypass the
predefined association between filenames and the corresponding document types.

