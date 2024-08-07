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

#### Importing Grammars

A grammar may import other grammars using the `scopeName` of the grammar to import. If another grammar is imported all
locally defined `patterns`, `highlight brackets`, `indent patterns`, `formatter`, `analyzers`, `evaluators` and `word character class`
definitions have preference over the correspondigly defined grammar aspects in the imported grammar(s).

Here is an example for a grammar for C-files importing a grammar defining defaults for languages with a C- like syntax
and grammar.

```json
 "scopeName": "cpp",
 "description":  "Grammar for C++- and C-code files",
 "import": [ "commonc" ],
```

### Templates
Templates are defined with a pattern `match` using to "select the template" and the actual `contents` of the template to be inserted. 
The contents of the template may contain variables enclosed in `${ }` - e.g. `${user}`.

Templates may have a `name` to be referred to by other templates. Special templates named `file` or `file_suffix` are used as file
templates and are applied, when a new file is created. Naming a template `file_suffix` can be used in situations, where files with
different extensions share the same grammar (e.g. `*.c` and include files `*.h`), if one wants to use as file templates depending
on the file extension. One would create a template named `file_h` to be applied to all files named `*.h` and a template named `file` 
to be used for all other files sharing the grammar containing the template.

Templates may be defined as `auto-insert` true, in which case the `match` pattern is matched against the currently typed input and
if a match occurs the template is automatically inserted. This auto-insertion behavior may be switched off for one document by
setting the `expandAbbreviations` option for the current editor tab to false [see configuring documents](document_types.md#editor_configuration_properties).

When a template is configured to be automatically inserted, one may define a lexical context, in which the automatic insertion
is performed:

- `lexical-context-initial` - the automatic insertion is not performed inside strings, character literals and comments.
- `lexical-context-comment` - the automatic insertion is only performed inside comments.

#### <a name="template-variables">Supported Template Variables</a>
The following variables are supported:

- `${user}` - name of the current logged in user
- `${cwd}` - the current working directory
- `${today}` - the current date and time
- `${file_created}` - the current file creation time
- `${file_modified}` - the current file modification time
- `${file_saved}` - the most recent time, when the current file was saved
- The following modifiers can be applied to all date types (today, file_created, ...)
  - `${date.date}` - the date part of the date (by default in format dd.mm.yyyy)
  - `${date.time}` - the current time
  - `${date.year}` - the current year (e.g. 2021)
  - `${date.year2}` - the last two digits of the current year (e.g. 24)
  - `${date.month}` - the current month as a number
  - `${date.month_name}` - the current month as a name
  - `${date.month_abbr}` - the current month as a abbreviated name
- `${pks_sys}` - the PKS_SYS directory
- `${copyright}` - currently active copyright profile template
- `${pks_tmp}` - the temporary directory for PKS Edit
- `${pks_executable}` - the PKS Edit executable full path
- `${indent}` - insert the number of spaces matching the indent of the line from which the template is inserted
- `${tab}` - the number of spaces to fill up one tab position
- `${cursor}` - after inserting the template, place the cursor here
- `${secondary}` - after inserting the template, place a secondary cursor here (up to 6 secondary carets are supported). This
   can be used to edit multiple places in the code concurrently / to start multi-caret input.
- `${file_name}` - base name of the current file (directory part removed)
- `${full_file_name}` - full file name of the current file
- `${file_name_no_suffix}` - base name of the current file with the suffix removed
- `${selection_end}` - used in combination with ${cursor} to select a range of text after inserting the template
- `${word_selection}` - the "word/identifier" close to the input caret or text currently selected
- `${template.name}` - nested template named _name_

One can apply functions on an expanded variable by adding a _function suffix_. Currently two functions are supported:

- `.toUpper()` - converts the expanded text to upper case
- `.toLower()` - converts the exanded text to lower case
- `.camelCase()` - converts the expanded text to camel case eliminating all spaces, `-` and `_` characters and starting
  with an upper case character after they have been found. `my_file` and `my file` and `my-file` are converted to `Myfile` for instance.

Example: `${file_name.toUpper()}` applied in a file named `c:\temp\myfile.txt` will evaluate to `MYFILE.TXT`.

Applying functions is currently not supported for `${template.name}`, `${copyright}`, `${word_selection}`.

### Bracket matching
PKS Edit supports navigation to `matching brackets` in source code. Matching brackets are typically defined as single
string pairs as in the following sample for a C code file, which defines opening and closing curly braces to pair:

```json
"highlightBrackets": [
    {
      "left": "{",
      "right": "}"
    }
]
```

The definition of matching brackets maybe however be a bit more tricky. Assume the following start- and end-tags
to be matched during ediuting from a LaTeX source file:

```latex
\begin{document} 
...
\end{document} 
``` 
Here the opening and closing brackets are to be defined via a regular expression as there might be multiple `begin{xxx}` and
matching end tags in a LaTeX code file. Moreover one wants to always match the *corresponding* end tag, which is defined by
the actual occurrence found when matching one side of the bracket (in our example `document`).

This can be expressed by defining a regular expression containing a match group, and by setting the highlight bracket definition
to be of type regular expression and by setting the `dynamic-match` property of a the bracket definition to true. The later will
works this way: if a match of a begi or end is found, the match group (`document`) is extracted from the match and the reverse
match is calculated by replacing the match group in the reverse match to the string found -> we will match `\end{document}` for instance.

```json
"highlightBrackets": [
{
  "left": "\\\\begin\\{([a-zA-Z0-9-]+)\\}",
  "right": "\\\\end\\{([a-zA-Z0-9-]+)\\}",
  "dynamic-match": true,
  "regex": true
}
],
```
