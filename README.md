# PKS Edit

The famous arcade times text editor from Pahlen & Krauß.

## History

PKS Edit was originally implemented on an Atari ST computer as 
a programmers editor in 1986 by Rolf Pahlen and Tom Krauß. The primary 
motivation for implementing it was the fact, the the built in editor on 
the Atari ST computer was simply to slow to allow for editing files any 
bigger than 500 lines long and Rolf and Tom were asked to migrate a 
Prolog interpreter written in Assembler (one file - about 30000 lines of code)
to the Atari ST platform. It was then sold as a programmers editor on
Atari ST.

Later a clone of PKS Edit named PKS Write was integrated into 
[Calamus](https://www.calamus.net/), a desktop publishing program still
running only on Atari ST.

It was then integrated as a text editor into [ObjectStudio Smalltalk](https://www.cincomsmalltalk.com/main/products/objectstudio/) and ported to Unix, Windows and OS/2.

Finally it was migrated to Microsoft Windows. The source code in this repo is the final
Windows version of the editor.

## Hacking

To change, edit, compile and run the source code of PKS Edit, we recommend to use
[Microsoft Visual Studio 2019](https://visualstudio.microsoft.com/de/vs/). The community
edition will do it - you do not need a professional or enterprise edition. Simply
open the Pksedit.sln solution and run / debug / build it.

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

To install PKS-Edit, copy the executable ``PKSEDIT.EXE`` along with the
directory PKS_SYS to a directory of your choice. The directory PKS_SYS
must be published in the ``WIN.INI`` file in an entry similar to the following:

```
[PksEdit]
PKS_SYS=C:\TOOLS\PKSEDIT\PKS_SYS
```

Usually you will use PKS-Edit by associating it with certain file types.
The registry file pksedit.reg contains an example on how 
to register some default types (*.java, *.txt) to be edited by PKS-Edit.

Enjoy Tom & Rolf