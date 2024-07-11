# Extras

This chapter describes the extra functions accessible from the _Extras_ menu.

## Start Command

Supports running a command (either batch or with UI) optionally processing in and outputs of the command.

- _Command_ the command to execute (e.g. `pksedit.exe` or `dir`).
- _Path_ the path, where the command will be started.
- _Redirections_ allows to redirect in- and or output from the command to the current active PKS Edit editor window allowing for use cases such as
  inserting a listing of file names into the text edited (using the DIR) command.
  - _Output_ - the output of the command will be inserted into the current editor window.
  - _Input_ - the input of the current editor window will be used as input for the command executed. If text is selected, only the selected text is used as input.
  - _In- and Output_ - the output **and** the input are re-directed from the current editor window.
- _Options_ defines options for running the command
  - _As Symbol_ - in particular applicable to applications opening a UI. Will run the application initially *iconized* (the application will
	not really open a window, but is initially displayed as an icon
  - <a name="start-command-wait"></a>_Wait_ - wait for the command execution to end and block PKS Edit while the command runs.
  - <a name="start-command-run-in-shell"></a>_Run in Command Shell_ - must be used for commands to be started in a `cmd shell` or `powershell` to execute (such as DIR or COPY or Get-Help).
	PKS Edit will automatically detect, whether the command must be invoked via `powershell` or Windows `CMD`.
- <a name="start-command-output"></a>_Redirect Output to_ - redirects the output to the named file and opens the file as a
  _PKS Edit Error List_ allowing to navigate potentials problems found running the command (typically a build command - maven or the like).

## Evaluate Code

Allows the evaluation of code: this is by default the text selected in the current editor window or all text in the current editor window. 
The Grammar of the file edited must support the evaluation of code (must have a known "evaluator" assigned). Languages, which can be evaluated
currently are of course PKS Macroc and DOS Commands.

## Read Tag File

Allows o open a file containing cross reference definitions (e.g. created by [CTAGS](https://docs.ctags.io/en/latest/index.html)). This
will read the cross references in this file and will then alllow to use them when navigating in PKS Edit.

## Read output produced by Build

This menu allows you to read a build log file, which is opened in the navigation pane and analyzed by PKS Edit for the purpose of navigating
to errors and warnings pointing to problems located in files. PKS Edit supports then to navigate the errors / warnings displayed in
the build log opened and opens the corresponding files / places, where the error occurred.

## Settings

Opens the PKS Edit Settings Dialog allowing to change the base configuration described in the corresponding 
[chapter](configuration.md).





