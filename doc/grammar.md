# PKS Edit

## Grammar Syntax

The grammar files of PKS Edit are used to adapt PKS Edit to editing different (in particular source) file types.
Basically a grammar file is used to define the following aspects of the behavior of PKS Edit:

- syntax highlighting. Each grammar contains a set of "pattern rules" which are used to select a font style to
  highlight areas of the code you are editing.
- templates. For each grammar one may define a set of code templates to be inserted on certain events by PKS Edit.
- bracket pairs. These can be defined for being checked automatically by PKS Edit.
- navigation patterns. These are used to navigate hyperlinks from one file to another (e.g. to navigate into an imported
  or included file).
- tag sources. These define the indices used for cross reference searches (ctags e.g. for the C programming language)
- code analysers. These are used to calculate additional suggestions for the suggestion / code completion functionality.

Every grammar resides in an own JSON file named 'grammar_id'.grammar.json and has a unique id (name), which is associated 
with a document type. The grammar files are placed in the PKS_SYS directory. The document types are defined in a file
PKS_SYS/pkseditconfig.json. Document types are typically selected by file name pattern. For every document type one may
define the grammar id to use for the corresponding document type.

### Templates
Templates are defined with a pattern using to "select the template" and the actual contents of the template. The contents
of the template may contain variables enclosed in ${ } - e.g. ${user}.

### Supported Template Variables
The following variables are supported:

- ${user} - name of the current logged in user
- ${date} - the current date (by default in format dd.mm.yyyy)
- ${year} - the current year (e.g. 2021)
- ${cursor} - after inserting the template, place the cursor here
- ${file_name} - base name of the current file
- ${selection_end} - used in combination with ${cursor} to select a range of text after inserting the template.
- ${word_selection} - the "word/identifier" close to the input caret or text currently selected.



