# PKS Edit

The famous Arcade Time Code Editor from Pahlen & Krauß originally released on the [Atari ST](https://en.wikipedia.org/wiki/Atari_ST)
and now available as a 64-Bit application for Windows 10 and Windows 11.

To follow the latest updates or to contribute visit the [PKS Edit Github Repository](https://github.com/TomKrauss/pks-edit).

## Changelog

-   Here is the link to the [Changelog](doc/CHANGELOG.md).

## Releases

- Installable versions of PKS Edit can be downloaded here:
  - [Release 2.10.0](https://github.com/TomKrauss/pks-edit/releases/tag/v2.10.0)
  - [Release 2.9.0](https://github.com/TomKrauss/pks-edit/releases/tag/v2.9.0)
  - [Release 2.8.1](https://github.com/TomKrauss/pks-edit/releases/tag/v2.8.1)
  - [Release 2.8.0](https://github.com/TomKrauss/pks-edit/releases/tag/v2.8.0)
  - [Release 2.7.0](https://github.com/TomKrauss/pks-edit/releases/tag/v2.7.0)
  - [Release 2.6.0](https://github.com/TomKrauss/pks-edit/releases/tag/v2.6.0)
  - [Release 2.5.0](https://github.com/TomKrauss/pks-edit/releases/tag/v2.5.0)
  - [Release 2.4.0](https://github.com/TomKrauss/pks-edit/releases/tag/v2.4.0)

## Features

PKS Edit supports many of the features supported by other code editors such as `Ultra Edit` or `Notepad++`,
but is typically much faster - a heritage of the old days, where CPU cycles mattered.

Here are some highlights:

- High performance: opening a code file with 1 million lines of code takes less then a second on a medium equipped machine. 
- Replacing every single character by an 'x' (replace regular expression '.' by 'x' honoring case sensitivity) on that same file takes about
  5 seconds, undoing this operation takes about 1 second.
- search & replace with many options (regular expression matching, preserve case on replace, etc...)
- search & replace in multiple files
- compare files
- syntax highlighting
- multi-caret editing
- user interface in English or German (other languages may follow)
- multiple views on single file
- cross references in code and references to other files and URLs (cross references in code requires a tag file - 
e.g. created using ctags)
- auto-indentation and some basic formatting
- sorting files
- code completion
- support for zooming (using keyboard and scrollwheel)
- record & play and a macro language allowing to edit the recorded macros
- different selection styles (block, line, normal)
- support for different file types and for user-defined grammars
- user defined templates
- hex editor mode for binary files
- support for themes - e.g. support for a dark theme.
- docking windows UI
- wysiwyg mode for [markdown](doc/markdown.md) (and limited HTML) documents (wysiwyg window opened as preview window of an editable document)
- support for tail -f mode to view large log files and watch them being appended
- additional simple coding features such as automatic bracket matching or auto-indenting or shifting or moving text blocks 
  or toggle comments for text blocks
- session management keeping track of previously opened files etc...
- build log file parsing and navigation to build output errors.

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
Install win_flex and win_bison into a directory GNU_HOME/bin and set the environment variable `GNU_HOME` to
point to that directory: The bin directory containing the files should be placed below GNU_HOME.

PKS Edit build needs the environment variable `GITLIBDIRECTORY` to point to the directory where the 
GIT library can be found (clone https://github.com/libgit2/libgit2 and refer to building it locally).
In order to build without GIT integration (no libgit2 present), you need to build configuration
`No Git` in Visual Studio.

To edit or execute the installation script (pksedit.iss), you need to download
[Inno Setup](https://jrsoftware.org/ishelp/index.php) (we use currently the latest version 6.2.2).

Enjoy Tom & Rolf
