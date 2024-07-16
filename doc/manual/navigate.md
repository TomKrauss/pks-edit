# Navigation

## Cross References

PKS Edit supports the navigation to cross references using a tags file as it can be created using [Universal Ctags](https://docs.ctags.io/en/latest/index.html).
In order to navigate cross references, the _grammar_ of the edited file must define the mechanism to navigate a cross reference. Mechanisms could also include
to perform a google search for the word under the cursor.

With "Find->Browse Cross Reference List..." one can open the list of known cross-references, with "Find->Goto Cross Reference at Caret" one can try to directly
navigate to the cross reference (word under the caret). If no matching cross reference is found in the later case, this will fall back to open the list of known
cross references.

PKS Edit by default looks for a file named `tags` located in the _current directory_ of the file currently edited. One can explicitly open a cross reference list
using menu "Extras->Open Tag File...".

## Auto-Bookmarks

PKS Edit sets automatic bookmarks

1. when navigating to other positions (search, traversal of links)
2. when starting a new editing operation

Using the menu "Find->Goto last position" or using the back icon in the toolbar one can navigate to the auto-bookmarks of type 1 and type 2
and using the menu "Find->Goto last insertion position" ome cam navigate to the auto-bookmarks
of type 2 only.

## Explicit Bookmarks

Using the "Find->Set Mark" and "Find->Goto Mark" one can set an explicit _named_ bookmark. A named bookmark is named using a single character,
which one can later specify in "Find->Goto Mark" to navigate to this _named_ bookmark.

## Go to Line
Using _Find->Goto Line..._ one may jump to a given 1-based line number. This is useful for instance if a compiler prints the line number
of error in a text. The _Goto Line_ Dialog will by default show the current line number and will reject illegal line numbers.



