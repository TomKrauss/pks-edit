## Grammars

A grammar is used by PKS Edit are used to adapt PKS Edit to editing different (in particular source) file types.
Every grammar has a name which can be associated with a [document type](document_types.md). 
Basically a grammar file is used to define the following aspects of the behavior of PKS Edit:

- `syntax highlighting rules`. Each grammar contains a set of rules which are used to select a font style to
  highlight areas of the code you are editing. The following types of matching rules are supported: 
  - _begin/end rules_: these are defined specifying a start string and an end string. They can be in particular used to
    match patterns, which span multiple lines as for instance needed for start and end multi-comments (e.g. Java or C).
  - _pattern rules_: these are defined using a regular expression and cannot match a string spanning multiple lines. They
    are best to match things like operators and identifiers
  - _keyword rules_: these are defined using an array of identifiers having a special meaning in a language.
- `templates`. For each grammar one may define a set of code templates to be inserted on certain events by PKS Edit.
- `bracket pairs`. These can be defined for being checked automatically by PKS Edit.
- `navigation patterns`. These are used to navigate hyperlinks from one file to another (e.g. to navigate into an imported
  or included file).
- `tag sources`. These define the indices used for cross reference searches (ctags e.g. for the C programming language)
- `code analyzers`. These are used to calculate additional suggestions for the suggestion / code completion functionality.
  Currently only one code analyzer is available, extracting all words / identifiers (not honorring syntax at all) from the 
  current text displayed in the editor. To use that analyzer in a grammar, add `"analyzer": "words"` to the grammar.
- The `wordCharacterClass`, which is used to define the borders of an identifier (used in cursor navigation and code completion)
and the translation table from lower case to upper case and vice versa (used during upper/lower case conversions). The following
word character class defines identifiers consisting solely of alpha characters and define lower to upper case `a-z=A-Z`. In a `wordCharacterClass`
all character (ranges) on the left side of the `=` sign are treated as lower case characters and all corresponding character (ranges) on
the right hand side are treated as the corresponding upper case characters. If there are identifiers, which do not fall in the category
lower/upper case they might be added to the right side of the `=` sign as the `-` character and all digits in the following character class `a-z=A-Z0-9-`.

Every grammar resides in an own JSON file named 'grammar_id'.grammar.json and has a unique id (name), which is associated 
with a document type. `cpp.grammar.json` for instance would be the name of a grammar named `cpp` (c++).
The grammar files are placed in the `pks_sys` directory. The document types are defined in a file
`pks_sys\pkseditconfig.json`. Document types are typically selected by file name pattern. For every document type one may
define the grammar id to use for the corresponding document type.

### Templates
Templates are defined with a pattern `match` using to "select the template" and the actual `contents` of the template to be inserted. 
The contents of the template may contain variables enclosed in `${ }` - e.g. `${user}`.

Templates may be defined as `auto-insert` true, in which case the `match` pattern is matched against the currently typed input and
if a match occurs the template is automatically inserted. This auto-insertion behavior may be switched off for one document by
setting the `expandAbbreviations` option for the current editor tab to false [see configuring documents](document_types.md#editor_configuration_properties).

When a template is configured to be automatically inserted, one may define a lexical context, in which the automatic insertion
is performed:

- `lexical-context-initial` - the automatic insertion is not performed inside strings, character literals and comments.
- `lexical-context-comment` - the automatic insertion is only performed inside comments.

#### Supported Template Variables
The following variables are supported:

- `${user}` - name of the current logged in user
- `${date}` - the current date (by default in format dd.mm.yyyy)
- `${year}` - the current year (e.g. 2021)
- `${indent}` - insert the number of spaces matching the indent of the line from which the template is inserted
- `${tab}` - insert the number of spaces to fill up one tab position.
- `${cursor}` - after inserting the template, place the cursor here
- `${file_name}` - base name of the current file
- `${selection_end}` - used in combination with ${cursor} to select a range of text after inserting the template.
- `${word_selection}` - the "word/identifier" close to the input caret or text currently selected.



