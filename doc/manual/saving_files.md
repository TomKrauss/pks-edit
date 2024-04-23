# Saving Files

You may save you changes at any time by pressing `Ctrl+S`. Note, that one may configure PKS Edit to automatically
save files. There are two autosave modes:

- _autosave to temp_ will have a copy of an edited file to be saved to the PKS Edit temp folder. When PKS Edit 
is closed (prematurely) and it is restarted and detects an autosaved file in the temp folder it will offer to
restore the changes from this temp file.
- _normal autosave_ will automatically save the current file after a configurable amount of time. Note, that you
should use this feature ideally in combination with a version control system allowing to restore the last version
to avoid unintentional overrides.