# PKS Edit

The famous arcade times text editor from Pahlen & Krauﬂ originally released on Atari ST
and here available as a 64-Bit application for Windows 7 and newer

## Release Notes

-   Here is the link to the [Release Notes](doc/release_notes.md)

## Features

The editor contains only some basic features compared to modern text editors such as 
`Ultra Edit`:

- search & replace with options like regular expression matching
- search & replace in multiple files
- compare files
- syntax highlighting
- edit one file in multiple windows
- follow cross references in code (requires a tag file - e.g. created using ctags)
- auto-indentation and formatting
- sort files
- code completion
- support for zooming font sizes (using keyboard and scrollwheel)
- record & play of macros
- it is pretty fast, if files do not get too big (no memory mapped files support yet)
- different selection styles (block, ...)
- support for different file types and for user-defined grammars
- support for inserting templates
- hex editor mode for binary files
- support for themes - e.g. support for a dark mode theme.
- docking windows UI

## Manual

- the [PKS-Edit Manual](doc/manual/toc.md) describes, how to work with PKS-Edit.

## Support for Markdown Display

PKS-Edit supports displaying markdown text in a [Wysiwyg mode](doc/markdown.md).

## Installing and Using PKS Edit

To install **PKS-Edit**, you may use the provided `INNO` Setup based installer.

Usually you will use PKS-Edit by associating it with certain file types.
The registry file pksedit.reg contains an example on how 
to register some default types (*.java, *.txt) to be edited by PKS-Edit.

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

To enhance the macro compiler, you need flex and bison. You may download flex and bison from
here:
- http://gnuwin32.sourceforge.net/packages/flex.htm
- http://gnuwin32.sourceforge.net/packages/bison.htm
Install flex and bison into a directory GNU_HOME and set the environment variable GNU_HOME to
point to that directory: The bin directory containing the files should be below GNU_HOME.

In order to edit the or execute the installation script (pksedit.iss), you need to download
[Inno Setup](https://jrsoftware.org/ishelp/index.php) (we use currently the latest version 6.2).

Enjoy Tom & Rolf
