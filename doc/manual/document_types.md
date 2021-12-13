# Document Types

The document types of PKS-Edit are configured in a file named `pkseditconfig.json` located in the `pks_sys` directory.

A document type is selected by one or more file name patterns (e.g. \*.cpp or \*.cpp;\*.c). Note, that _more specific_ file name patterns are matched first (\*.c matches in preference over \*.\*).
Each document type may be associated with an editor configuration (also by name) defining things like the standard display mode or tabstops, [insertion options](editing_files.md#editing-text) and
a _grammar_ which is referred to be name.

## Grammar

A grammar is defined by a name and configured in corresponding JSON file name `grammarName`.grammar.json.

- Format of [Grammar and Template Files](grammar.md)

## Editor Configuration
The following example defines an editor configuration `default`, which defines some standard tabstops, margins, tabsizes, and options for editing and displaying text.

```
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

Here is a description of some of the defined properties of an `editor configuration`.

- `codepage` - the standard character encoding / code page for documents of this type. Specify either a WIN32 code for the desired encoding (0 == ISO8859-1 or 
   65001 == UTF-8) or -1 to have PKS-Edit "guess" the character encoding.
- `shifWidth` the number of columns defining one `logical edited tab` - i.e. the number of columns by which a selected range of text is indented, when pressing Tab.
- `tabSize` - the number of columns defining one `logical visible tab`
- `showWhitespace` - whether whitespace characters (tab, spaces, line-ends) are displayed during editing.
- `autoIndent` - whether entering a new line should try to automtically insert the "proper number of tabs" at the beginning of the new line depending
  on the number of tabs displayed in the prvious line etc...

## Example
Here is an example of a document type descriptor for C and C++ files, which associates the files with a grammar named `cpp` and connects the files with a `default` editor configuration.

```
{
    "documentTypes": [
    {
        "name": "c+cpp",
        "description": "C/ C++ files (*.c,*.h,*.cpp,*.hpp)",
        "grammar": "cpp",
        "editorConfiguration": "default",
        "filenamePatterns": "*.c;*.h;*.cpp;*.hpp"
    },
...

```

