## Macros in PKS-Edit

Macros in PKS-Edit can be recorded or manually edited using the _PKSMacroC Language_ and compiled inside PKS-Edit.
All known macros are loaded from a file called **pksedit.mac** loaded from the `PKS_SYS` directory.

The macro file contains currently:

- custom macros performing text operations
- keyboard, mouse and menu bindings to PKS-Edit commands and macros

## Redefining keyboard bindings

To redefine keyboard bindings use the following sequence of commands:

- Generate the list of current keyboard bindings using menu Macro->View Bindings->Keyboard
- In the file, which opens, change the bindings. **Note**: you may use code completion to change the desired command and you may use Macro->Insert Keycode to
insert a keyboard keycode into the file.
- When done editing, press F10 an compile the new keybinding file.
- Finally save the current set of key bindings and macros using Macro->Save... Save the results in a file named `pks_sys\pksedit.mac`.

## PKSMacroC Language

The PKS-Edit Macro Language is pretty similar to the C programming language and supports the most common operators 
and functions and variable declarations as a C program with some modifications and extensions. There is no explicit memory management
necessary - the macro interpreter performs all object allocations automatically for you.

Here is sample of a piece of macro code, which will calculate the factorial of a number to enter and display the
result in a message box:

```
#
# Calculate the factorial of a number input into a message box.
# Note: PKSMakroC - Integers are only 24 bit big, this macro
# will cause overflows for fac(>12) !
#
macro calculateFactorial() {
	int		count = 1;
	int		n = 1;
	int		save_n = 1;
	int		res = 1;
	string	result = "";

	n = (int)PromptAssign("Calculate factorial of: ","10");
	save_n = n;

	while(n > 1) {
		res = res * n;
		n = n - 1;
	}

    if (res < 0) {
        MessageBox(0, MB_ICONERROR|MB_OK, "Overflow during calculation.");
    } else {
	    result = "The factorial of " + (string) save_n + " is: " + (string) res;
        MessageBox(0, MB_ICONINFORMATION|MB_OK, result);
    }
}

```
