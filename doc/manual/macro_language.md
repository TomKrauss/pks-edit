# Recording Macros and the PKS-Edit Macro Language

## Macros in PKS-Edit

Macros in PKS-Edit can be recorded or manually edited using the _PKSMacroC Language_ and compiled inside PKS-Edit.
All known macros are loaded from a file called **pksedit.mac** loaded from the `pks_sys` directory.

The macro file contains currently:

- custom macros performing text operations - for instance used as post-save actions or the like.

## Recording macros

To record a macro, use "Macro->Record" and start doing some actions (e.g. move the cursor, insert text, etc...). When done press "Macro->Record" again. This will open a little dialog
allowing to "name" the macro and bind it to a keyboard key so it can be used subsequently. Note, that in order to make the macro persistent, you must save the the current set of macros.
When PKS-Edit exits and there are unsaved macros, it warns about that and asks the user to save the macros now.

## The Maco menu

Allows you to edit macros defined. One may add comments / remove macros, decompile a recorded macro, disassemble a macro from here.

## Compiling macros

To compile one or more macros, you open a macro file (see the `macros` sub-directory for samples) and press F10 once the file is loaded and then OK. If the compilation is successful
a message is displayed, that no error was found. Otherwise an error log is opened, which can be navigated with Ctrl+N and Ctrl+P to jump to the corresponding line numbers with the
erors.

## The PKSMacroC Language

The PKS-Edit Macro Language is pretty similar to the C programming language and supports the most common operators 
and functions and variable declarations as a C program with some modifications and extensions. There is no explicit memory management
necessary - the macro interpreter performs all object allocations automatically for you.

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

## PKSMacroC Types

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
- `string[]`, `int[]`,.... an array may contain 0 to n nested values currently and is typically defined using an array literal such as `["hello", "world"]`. Array
literals with other element types are currently not yet supported.
- `map`s can be used to associate arbitrary values with string type keys (so far only string type keys are supported). To put a value into a map
   use map["key"] = value, to access a value in a map use map["key"]. 
 
PKSMacoC will try to coerce values to their "right" types or you may `cast` a value type to another explicitly using a cast operator. The following examples
shows implicit and explicit coercions.

```
(int)"123" -> 123
(string)1 -> "1"
// implicit coercion
"Hello " + 3 + " worlds"  -> "Hello 3 worlds"
```

## PKSMacroC literals

PKSMacroC supports **literals** for defining int, string, boolean, float, char, string and array type values.

- `int` values can be defined using either decimal, hexadecimal, octal or binary representation using a C like syntax as for example: 
  `-42` (decimal) `0xA0` (hexadecimal) `071` (octal) `0b10010100` (binary). To make long numbers more readable, one may inject underscores. One million
  could be written as example as 1_000_000.
- `float` type values currently only support the simple syntax [sign][digits].[digits] as in `3.14`.
- string type literal are enclosed in `"`. Special characters such as `"`, `\n`, `\r`, `\f`, `\\` ,... (see C syntax) must be quoted using`the `\` character.
  Examples are "hello\nworld" or "this is my \"favorite\" movie"
- array type literal support currently only the specification of simple arrays - not nested ones. The individual elements are enclosed in 
 `{`and `}` and seperated  by `,`. Examples: `{ "hello", "world" }` or `{ true, 42, "oscar"}`.
- Literals for maps are not yet fully supported. They are enclosed in square brackets and the key and values are associated using `=>` as in:
  `[ "key" => 42, "key" => "valueforKey2", ...]`

## PKSMacroC operators

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
  
## PKSMacroC constants

You can define constants outside of macros for convenient naming of typical literals. Constants are inlined
when a macro is compiled as their respective literal and cannot be changed as the name implies.

Here is an example for a declaration of some common constants:

```
const TAB = '\t';
const PI = 3.14159265359d;
```

## PKSMacroC control flow

PKSMacroC currently supports the following control flow expressions:

- `while (xx) { ... }` - allows to implement a while loop. While loops support nested `break` and `continue` statements.
- `goto label` allows you to jump to a given label defined with `label_name:`.
- `if (condition) ... [else ...]` - allows to conditionally execute one block with an optional else clause.
. 'for(type var : expression) {}` - a foreach loop iterating over the elements of the result of evaluating `expression` setting the 
   variable `v` to the elements produced. Currently one can iterate strings and ranges.
- `switch(var) ... [case ...: statements]` - an almost classical switch statemen (see below).

### PKSMacroC switch expressions

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



 
