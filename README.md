# PKS Edit

The famous arcade times text editor from Pahlen & Krauß.

## Features

The editor contains only some basic features compared to modern text editors such as 
Ultra Edit:

- search & replace with options like regular expression matching
- search & replace in multiple files
- compare files
- sort files
- follow cross references in code (requires a tag file - e.g. created using ctags)
- auto-indentation and formatting
- record & play of macros
- it is pretty fast, if files do not get too big (no memory mapped files support yet)
- different selection styles (block, ...)
- basic support for different file types
- and some more - but that's it

## Installing and Using PKS Edit

To install PKS-Edit, you may use the provided INNO Setup based installer.

Usually you will use PKS-Edit by associating it with certain file types.
The registry file pksedit.reg contains an example on how 
to register some default types (*.java, *.txt) to be edited by PKS-Edit.

## Hacking

To change, edit, compile and run the source code of PKS Edit, we recommend to use
[Microsoft Visual Studio 2019](https://visualstudio.microsoft.com/de/vs/). The community
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

## Release Notes

-   Here is the link to the [Release Notes](doc/release_notes.md)

Enjoy Tom & Rolf
