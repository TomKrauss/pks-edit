# Document Types

The document types of PKS Edit are configured in a file named `pkseditconfig.json` located in the `pks_sys` directory.

A document type is selected by one or more file name patterns (e.g. \*.cpp or \*.cpp;\*.c). Note, that _more specific_ file name patterns are matched first (\*.c matches in preference over \*.\*).
Each document type may be associated with an editor configuration (also by name) defining things like the standard display mode or tabstops, [insertion options](editing_files.md#editing-text) and
a _grammar_ which is referred to by name.

## Grammar

A grammar is defined by a name and configured in corresponding JSON file name `grammarName`.grammar.json.

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
Here is a description of some of the defined properties of an `editor configuration`.

- `codepage` - the standard character encoding / code page for documents of this type. Specify either a WIN32 code for the desired encoding (0 == ISO8859-1 or 
   65001 == UTF-8) or -1 to have PKS Edit "guess" the character encoding.
- `actionContext` - a context to use for documents of the described types for the bound _actions_. Every PKS Edit action binding (keyboard binding, mouse binding
  menus, ...) can have an _action context_ in which the corresponding binding is active. If no action context is defined explicitly a default action context
  is assumed.
- `shiftWidth` the number of columns defining one `logical edited tab` - i.e. the number of columns by which a selected range of text is indented, when pressing Tab.
- `tabSize` - the number of columns defining one `logical visible tab`
- `showWhitespace` - whether whitespace characters (tab, spaces, line-ends) are displayed during editing.
- `expandAbbreviations` - if set to true, templates defined in the [grammar](grammar.md) which are marked with `auto-insert=true` will be
  automatically expanded, when a match with the template pattern is encountered during text input.
- `showSyntaxHighlight`- if set to true, will have the text of documents with a defined grammar being colored using the styling rules defined in the grammar.
- `showLineNumbers` - if set to true, displays the line number area of a editing window. This will (as the name suggests) display the line numbers, but also
  additional things like the change annotations.
- `highlightCaretLine` - if set to true, displays the line in which the input caret is placed using a slightly different background color making 
  the input caret easier to detect.
- `smartSpaceDelete` - can be set to true in particular in the case tabs are "emulated" using spaces to have backspace delete multiple spaces at once
  pretending one would delete a tab character.
- `fillCharacter` - if set to a value != 0, will have all tabs being expanded to the assigned character.
- `autoIndent` - whether entering a new line should try to automtically insert the "proper number of tabs" at the beginning of the new line depending
  on the number of tabs displayed in the prvious line etc...
- `tabulatorSizes` - can be assigned to an array of values used as the respective tab stop sizes. If the array size is 1, the single contained 
  element defines the tabstop size.
 
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

