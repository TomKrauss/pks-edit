# PKS Edit

The famous arcade times text editor from Pahlen & Krauß originally released on Atari ST
and now available as a 64-Bit application for Windows 7 and newer

## Changelog

-   Here is the link to the [Changelog](doc/CHANGELOG.md)

## Releases

- Installable versions of PKS Edit can be downloaded here:
  - [Release 2.4.0](https://github.com/TomKrauss/PKS%20Edit/releases/tag/v2.4.0)
  - [Release 2.3.0](https://github.com/TomKrauss/PKS%20Edit/releases/tag/v2.3.0)
  - [Release 2.2.0](https://github.com/TomKrauss/PKS%20Edit/releases/tag/v2.2.0)
  - [Release 2.1.0](https://github.com/TomKrauss/PKS%20Edit/releases/tag/v2.1.0)

## Features

The editor contains only some basic features compared to modern text editors such as `Ultra Edit`.

Here are some highlights:

- it is very fast, if files do not get too big (no memory mapped files support yet). Opening a code file
  with 1 million lines of code takes less then a second on a medium equipped machine. Replacing every single character
  by an 'x' (replace regular expression '.' by 'x' honoring case sensitivity) on that same file takes about
  5 seconds, undoing this operation takes about 1 second.
- search & replace with options like regular expression matching
- search & replace in multiple files
- file comparison
- syntax highlighting
- multi-caret editing
- user interface in English or German (other languages may follow)
- multiple views on single file
- cross references in code and references to other files and URLs (cross references in code requires a tag file - 
e.g. created using ctags)
- auto-indentation and some basic formatting
- sorting files
- code completion
- support for zooming font sizes (using keyboard and scrollwheel)
- record & play and a macro language allowing to edit the recorded macros
- different selection styles (block, line, normal)
- support for different file types and for user-defined grammars
- user defined templates
- hex editor mode for binary files
- support for themes - e.g. support for a dark theme.
- docking windows UI
- [wysiwyg mode](doc/markdown.md) for markdown documents (currently readonly, typically wysiwyg window opened as 2nd view of an editable document)
- support for tail -f mode to view large log files and watch them being appended
- additional simple coding features such as automatic bracket matching or auto-indenting or shifting or moving text blocks 
  or toggle comments for text blocks
- session management keeping track of previously opened files etc...

## Manual

- the [PKS Edit Manual](doc/manual/toc.md) describes, how to work with PKS Edit.

## Examples

The examples directory contains several [coding samples](examples/sample.md) demonstrating the features
of PKS Edit when it comes to using it to edit files of a given supported type.

## Installing and Using PKS Edit

To install **PKS Edit**, you may use the provided [Inno Setup](https://jrsoftware.org/ishelp/index.php) based installer. The
installer will associate typical file extensions and make PKS Edit the default editor for those extensions if you request it
to do so.

## Cross References in source files and PKS Edit

PKS Edit supports the [Universal Ctags](https://docs.ctags.io/en/latest/index.html) format
for navigating cross references. By default PKS Edit will try to locate a file named "tags" in
the current working directory to navigate cross references. One may navigate a reference by
clicking with Ctrl+Left Mouse Button onto a word in the source code.

## Hacking

To change, edit, compile and run the source code of PKS Edit, we recommend to use
[Microsoft Visual Studio 2022](https://visualstudio.microsoft.com/de/vs/). The community
edition will do it - you do not need a professional or enterprise edition. Simply
open the Pksedit.sln solution and run / debug / build it.

To enhance the macro compiler, you need win__flex and win__bison. You may download flex and bison from
here. We currently use the latest version 2.5.25.
- https://github.com/lexxmark/winflexbison
Install win_flex and win_bison into a directory GNU_HOME/bin and set the environment variable GNU_HOME to
point to that directory: The bin directory containing the files should be placed below GNU_HOME.

In order to edit the or execute the installation script (pksedit.iss), you need to download
[Inno Setup](https://jrsoftware.org/ishelp/index.php) (we use currently the latest version 6.2).

Enjoy Tom & Rolf
