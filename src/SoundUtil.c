/*
 * SoundUtil.c
 *
 * Project: PKS Edit for Windows
 *
 * purpose: play an error tone.
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <windows.h>
#include "stringutil.h"
#include "editorconfiguration.h"

#define BEEP_DURATION 80

/*--------------------------------------------------------------------------
 * sound_playChime()
 * Play a chime sound.
 * TODO: play different sounds depending on severity of error, which occurred.
 */
void sound_playChime(void) {
	EDITOR_CONFIGURATION* pConfig = GetConfiguration();
	if (_stricmp("beep", pConfig->soundName) == 0) {
		Beep(770, BEEP_DURATION);
		return;
	}
	if (_stricmp("lowbeep", pConfig->soundName) == 0) {
		Beep(220, BEEP_DURATION);
		return;
	}
	if (_stricmp("highbeep", pConfig->soundName) == 0) {
		Beep(5000, BEEP_DURATION);
		return;
	}
	if (!pConfig->soundName[0]) {
		PlaySound((LPCSTR)SND_ALIAS_SYSTEMASTERISK, NULL, SND_ASYNC | SND_ALIAS_ID | SND_SYSTEM);
	} else {
		PlaySound(TEXT(pConfig->soundName), NULL, SND_ASYNC | SND_SYSTEM);
	}
}

