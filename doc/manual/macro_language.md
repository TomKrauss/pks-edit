# Recording Macros and the PKS-Edit Macro Language

## Macros in PKS-Edit

Macros in PKS-Edit can be recorded or manually edited using the _PKSMacroC Language_ and compiled inside PKS-Edit.
PKSMacroC compiles files (by default with an extension .pkc) into a binary format, which can be saved and loaded
by PKS-Edit. By default a file called **pksedit.mac** is loaded from the `pks_sys` directory containing the standard
macros available in PKS-Edit.

The standard macro file shipped with PKS-Edit currently contains:
- custom macros performing text operations - for instance used as post-save actions or the like.

## Recording Macros
To record a macro, use "Macro->Record" and start doing some actions (e.g. move the cursor, insert text, etc...). When done press "Macro->Record" again. This will open a little dialog
allowing to "name" the macro and bind it to a keyboard key so it can be used subsequently. Note, that in order to make the macro persistent, you must save the the current set of macros.
When PKS-Edit exits and there are unsaved macros, it warns about that and asks the user to save the macros now.

## The Macro Menu
Allows you to edit macros defined. One may add comments / remove macros, decompile a recorded macro, disassemble a macro from here.

Also contains menu entries created from the macros defined allowing to use any defined macro.

## Compiling Macros

To compile one or more macros, you open a macro file (see the `macros` sub-directory for samples) and press F10 once the file is loaded and then OK. If the compilation is successful a message is displayed, that no error was found. Otherwise an error log is opened, which can be navigated with Ctrl+N and Ctrl+P to jump to the corresponding line numbers with the erors.

## The PKSMacroC Language

### PKSMacroC Design
PKSMacroC as the name suggests has a C like syntax (not object oriented in particular) with some ideas borrowed from JavaScript
and Java such as a JSON like syntax for declaring maps, spread operators, annotations and Java like numeric literals.

### Syntax Overview
The PKS-Edit Macro Language supports the most common operators  and functions and variable declarations as a C program. 
There is no explicit memory management necessary - the macro interpreter performs all object allocations and deallocations automatically for you.

Here is a sample showing a piece of macro code, which will calculate the factorial of a number to enter and display the
result in a message box:

```
/*
 * Calculate the factorial of a number input into a message box.
 * Note: PKSMakroC - PKS MacroC Integers are 64 bit big, this macro
 * will cause overflows for fac(>16) !
 */
void calculateFactorial() {
    int     nSelected = 1;
    int     result = 1;

    nSelected = (int)PromptAssign("Calculate factorial of: ","10");
    if (nSelected == 0) {
        MessageBox(MB_ICONINFORMATION|MB_OK, "Calculation aborted.");
        return;
    }

	for (int n : 1..nSelected) {
        result *= n;
	}

    if (result < 0) {
        MessageBox(MB_ICONERROR|MB_OK, "Overflow during calculation.");
    } else {
        MessageBox(MB_ICONINFORMATION|MB_OK, "The factorial of " + nSelected + " is: " + result);
    }
}
```

## PKSMacroC Comments
PKSMacroC supports - as C or Java single line comments introduced using `//` as in the following sample:
```
   int i = 0;       // initialize although 0 is the default value of int values
```

One may also use multiline comments as in C or Java enclosed in `/*` and `*/`.  If a multiline
comment is placed in front of a MacroC function declaration, it is used internally (during code completion etc...)
as the main description of the described function. Here is an example:
```
/*
 * Macro, which can be used as a save action in PKS-Edit to trim
 * all trailing blanks in the document saved.
 */
void TrimTrailingBlanks() {
...
```

## Types

**PKSMacroC** supports 3 types of commments:

- `#` as first character introduces single-line comments
- `//` also introduces single-line comments
- `/*` and `*/` may introduce multi-line comments similar to C. If one places a multi-lane comment before a macro, the comment will be used as 
  the description / help (in code completion) text for that macro.
 
All primitive values (int, string, float, bool, range) in **PKSMacroC** code are `immutable`, array and structured types may be changed. 
Values may have one of the following types:

- `string` as the name implies a string (internally implemented using zero terminated strings). Strings in PKS-Edit are dynamically allocated and de-allocated.
  no memory management is necessary in PKSMacroC code. One can concatenate strings by using the '+' operator. `string` literals follow C syntax and are enclosed
  in `"´` characters.
- `int` represents integer numbers in a 64 bit value range. `int` literals may be written as either decimal numbers (`123`), hexadecimal numbers (`0xAF34`) or
  octal numbers (`\0234`).
- `float` represents floating point numbers. Floating point literals contain either a `.` character and may have a trailing `d` suffix (`12d`).
- `boolean` boolean types are very often not explicitly defined, but created temporarily by executing a logical / relational operator on other types.
- `range` ranges can be used to select a range of text or they can be iterated over using the foreach expression. A range has a lower and an upper bounds and
   an optional increment. Ranges are built using the `..` operator as in `1..5` (Numbers 1 to 5 inclusively) or `0..21..3` (Numbers from 0 to 21 inclusively
   but with an increment of 3).
- `string[]`, `int[]`,.... an array may contain 0 to n nested values currently and is typically defined using an _array constructor_ such as `["hello", "world"]` or
`[ true, 4711, i ]` (creates an array containing the values `true`, `4711` and the current value of the variable `i`).
- `map`s can be used to associate arbitrary values with string type keys (so far only string type keys are supported). To put a value into a map
   use map["key"] = value, to access a value in a map use map["key"]. Maps can also be created using a _map constructor_, which has a syntax like
   in the following sample `{ "x": 42 }` (a map with one association of "x" with the value 42) or `{ "a": v1, "b": "anotherString" }` (map
   with two associations "a" with the value of variable v1 and "b" associated with the string "anotherString").

PKSMacoC will try to coerce values to their "right" types or you may `cast` a value type to another explicitly using a cast operator. The following examples
shows implicit and explicit coercions.

```
(int)"123" -> 123
(string)1 -> "1"
// implicit coercion
"Hello " + 3 + " worlds"  -> "Hello 3 worlds"
```

## Literals

PKSMacroC supports **literals** for defining int, string, boolean, float, char and string type values. Maps and arrays may be specified using _constructor_
expressions, which are similar to literals but support also enclusion of variable references and sub-expressions.

- `int` values can be defined using either decimal, hexadecimal, octal or binary representation using a C like syntax as for example: 
  `-42` (decimal) `0xA0` (hexadecimal) `071` (octal) `0b10010100` (binary). To make long numbers more readable, one may inject underscores. One million
  could be written as example as 1_000_000.
- `float` type values currently only support the simple syntax [sign][digits].[digits] as in `3.14`.
- string type literal are enclosed in `"`. Special characters such as `"`, `\n`, `\r`, `\f`, `\\` ,... (see C syntax) must be quoted using`the `\` character.
  Examples are "hello\nworld" or "this is my \"favorite\" movie"
- arrays may be specified using array _constructor_ expressions. The individual elements are enclosed in 
 `[`and `]` and seperated  by `,`. Examples: `[ "hello", "world" ]` or `[ true, 42, "oscar"]`. Note, that this syntax supports also enclosing
 expressions and the use of variables as in `[var1, 1+2]`.
- Maps may also be specified using a _constructor_ statement. They are enclosed in curly brackets and the key and values are associated using `:` (JSON like syntax) as in:
  `{ "key": 42, "key": "valueforKey2", ...}`

## Operators

PKSMacroC supports the most common operators also supported in the C language.

- `+`, `-`, `*`, `/`, `%` arithmetic operators
- `!`, `<`, `<=`, `>`, `>=`, `==`, `!=`, `&&`, `||`  logical operators
- `&`, `|`, `^` bitwise operators
- `**`  power to operator to be applied to float or int data types
- `..` the range operator allowing to construct a range type from two integer values. `3..5` creates a range from 3-5. See string functions for more details.
 
One of the strengths of PKSMacroC are the special string manipulation operators supported:

- string1 `+` string2 - will contatenate the two strings. If only one of the operands is a string, the other one is automatically
  coerced to a string.
- string1 `-` string2 - will remove the 1st occurrence of string2 from string1. "abcd" - "bc" -> "ad". 
- `~`, `!~` _matches_ / _does_not_match_ operator. Can be used to match a string with a regular expression (`"aa" ~ "[a]+"`)
- multiplier `*` stringOrChar - can be applied on character and string arguments and creates a resulting string containing multiplier occurrences of the string passed.
- `[numOrRange]` - can be used to get a single char of a string or a range of characters. "hello"[1] will result in 'e' and "hello"[0..3] will result in "hel".

Operators can also be used on `string[]` type values:

- array `+` string append a string to the array and return a new array.
- array `+` array2 append an array2 to the 1st array.
- array`[0]` return the first element of an array
- array`[range]` returns the elements matching range. Using a range of 1..8..2 (range starting from one including 8 with an increment of 2) 
  will extract elements 2, 4, 6, 8.
  
**PKSMacroC** also allows to use the typical C assignment shorthand notations such as

- `i++`, `i--` increment or decrement operators
- `i += x`, `i *= x`, ... shorthand assignment and calculation operators.
- one can access the characters of strings using the `[]` operator such as in C.

### The Spread Operator

The spread operator can be used to expand an array into a parameter list of a function or an array constructor. 
Let's look at the following example:

```
myArray = [editor, 1];
EditorGotoLine(...myArray);
// equivalent to call EditorGotoLine(editor, 1);
```

The spread operator is in particular useful for appending arrays to other arrays.

```
array1 = ["a", "b"];
array2 = [1,2, ...array1];
// -> array2 == [1,2,"a","b"];
```

## Constants

You can define constants outside of macros for convenient naming of typical literals. Constants are inlined
when a macro is compiled as their respective literal and cannot be changed as the name implies.

Here is an example for a declaration of some common constants:

```
const TAB = '\t';
const PI = 3.14159265359d;
```

## Control Flow Statements

PKSMacroC currently supports the following control flow expressions:

- `while (xx) { ... }` - allows to implement a while loop. While loops support nested `break` and `continue` statements.
- `goto label` allows you to jump to a given label defined with `label_name:`.
- `if (condition) ... [else ...]` - allows to conditionally execute one block with an optional else clause.
- `for (initializer; condition; incrementExpression) {...}` - regular C-style for loop. E.g. 
```
    for (int i = 0; i < 100; i += 3) {
        println(i);
    }
```
- `for(type var : expression) {}` - a foreach loop iterating over the elements of the result of evaluating `expression` setting the 
   variable `v` to the elements produced. Currently one can iterate strings and ranges.
- `switch(var) ... [case ...: statements]` - an almost classical switch statemen (see below).

### Switch Expressions

In PKSMacroC, case labels must uniquely select the statements to execute or a compile error is displayed. One may specify
either a number, a number range (a..b), a string or the default case, Case labels must uniquely select a block of statements
to execute. Here is an example for a PKSMacroC switch statement.

```
switch (myNumber) {
    case 1: println("1"); break;
    case 2..5: println("range selected"); break;
    // here you will get a compile error: 2..5 overlaps 3..6
    case 3..6: println("range selected"); break;
    default: println("DEFAULT");
}

switch (myString) {
    case "##":  println("double hash"); break;
    case "--":  println("double dash"); break;
}

```

## Namespaces, Visibility and Require

Support for namespaces and visibility is currently of limited use. 

You define the `namespace` in which one or more macros are defined using a `namespace declaration` at the beginning 
of the file as in the following sample.

```
namespace unittest;
``` 

If no explicit namespace is defined, a namespace named `default` is active.

You may make a macro method `static` to restrict its visibility. This restriction is currently only used to make the 
corresponding function not visible by default at places where macro fuction names are displayed in the UI (macro editor, action list, etc...). 
You should not try to use a static macro in an action binding. This will work currently but not on a long term run. 
On a long term run static methods can only be called by methods defined in the same `namespace`.

You may import declarations from another namespace (see caveats for comments) using the `require` statement. Here
is an example:

```
require "unittest";
```

Note, that require has currently several limitations, one of which is, that the required namespace is only found, if it is 
defined in a file called _namespacename_.pkc. You may however also require a file name directly. File pathes must always be
relative to the path of the file using the require statement (or absolut, which is not recommended).

## Defining Native Methods

One can extend PKSMacroC to some extent using native method bindings. A native method binding is defined
in the header of a PKSMacroC file as shown in the following example:

```
/*
 * Retrieves the calling thread's last-error code value. The last-error code is maintained on a per-thread basis.
 * Multiple threads do not overwrite each other's last-error code.
 */
@Method(lib="kernel32.dll")
native int GetLastError();
```

The shown examples makes it possible to invoke "GetLastError" from PKSMacroC code to invoke the corresponding WIN32 API
from kernel32.dll. Adding native method binding has currently quite some limitations - only simple parameter types
are supported.

The next example is used to document a defined PKS-Edit standard native method:

```
/*
 * Inserts the passed string into the currently active editor or into the editor passed as 1st parameter.
 */
@Method(name="EditorInsertString")
native int edit_insertString(EDITOR anEditor, string aString);
```

Note that this time the `@Method` annotation does not need to specify the lib from which the method is loaded,
as it is loaded from the PKS-Edit exe itself. Here we specify an alternate API name to be used inside PKSMacroC (EditorInsertString
instead of edit_insertString, which is the corresponding native method name).

Finally one may pass information about the signature of the method not expressible in PKSMacroC syntax. 
The following example defines the MacroC `signature` of the method also specifying enum values which can be used for
parameters.

```
/*
 * Replace all occurrences of the search string with the replacement parameter using
 * the specified regular expression flags in a given range. Besides replacing this method
 * may also perform an alternate replacement action defined using the last argument.
 */
@Method(name="EditorReplaceText", signature="iWssbRE_eRNG_eREP_")
native int edit_replaceText(EDITOR anEditor, string aSearch, string aReplace, int reOptions, int nRange, int nReplaceActions);
```

### Signatures

Signature define the way, macros and functions may be called in the PKSMacroC language. Signatures are by default defined through the
declaration of a native method (return type + parameter types). To describe special call behavior, one may defined an explicit signature 
parameter on the @Method annotation. In the explicit signature definined, the 1st signature token defines the return type of the method.
Subsequent tokens define the type of the call parameters.

Signature tokens may be:

- i - parameter / return type is an integer
- s - parameter / return type is a string
- W - parameter / return type is an EDITOR handle
- b... - parameter / return type is a bit set of enum values starting with ... (e.g. `bRE_` a bit combination of regular expression options)
- e... - parameter / return type is a single enum value starting with ... 
- P... special token to mark the method a natively defined method (used internally in PKS-Edit for more advanced handling of parameters)

## Optional Parameters
Some native methods - in particular those operating on an editor support passing the editor (type `EDITOR`) as the first argument of
the method to invoke. An example is the method `EditorReplaceText(EDITOR editor, string pattern, string replaceWith, ...)`. In these
cases the first parameter (`editor`) is optional. If no editor is passed, the currently active (focussed) editor is used as a default
value.

## Annotations
Native method (as shown before) and macro definitions can be annotated to add _meta-data_ information to the native method or macro. Currently only a limited
set of annoations is supported:

- `@Method` this annoation can be added to native method definitions primarily to document the native method (see above for details).
- `@ActionFlags` this annotation can be added to a macro to control the behavior of actions (menu entry, toolbar actions etc...) created
  with the macro. One can use the nested properties
  - `needsCurrentEditor` can be used to express, that an action for this macro is only available, if there is a current editor
  - `needsSelection` can be used to express, that an action for this macro is only available, if text is selected in the current editor
  - `modifiesText` can be used to express, that this action will modify the text
  - `ignoreDuringRecording` can be used to express, that the execution of this macro will not be recorded by the macro recorder
  - `undoAvailable` can be used to express, that the execution of this macro requires undo to be possible.

## Caveats

PKSMacroC currently does not support short-circuiting boolean expressions (McCarthy evaluations). In the expression `if (len < 10 && array[len] == c)` 
`array[len] == c` is **always evaluated** regardless of the outcome of the test `len < 10`. We will fix this later to make boolean expressions more
powerful and simpler to write.

The `require` keyword does not work as expected. It will load the "required" source **after** the file requiring it was loaded. We will fix that as
well on a long term run in particular when introducing the ability to define own types (enums and structs) to be able to load declarations before
they are used.


