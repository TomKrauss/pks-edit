/*
 * SoundUtil.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS
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

static char _soundName[32];

/*--------------------------------------------------------------------------
 * sound_initialialize()
 */
static void sound_initialialize() {
	static int 	done;

	if (done)
		return;
	done = 1;
	prof_getPksStandardString("ErrorSound",
		_soundName, sizeof _soundName);
}

/*--------------------------------------------------------------------------
 * sound_playChime()
 * Play a chime sound.
 * TODO: play different sounds depending on severity of error, which occurred.
 */
void sound_playChime(void) {
	sound_initialialize();
	if (!_soundName[0]) {
		PlaySound((LPCTSTR)SND_ALIAS_SYSTEMASTERISK, NULL, SND_ASYNC | SND_ALIAS_ID | SND_SYSTEM);
	} else {
		PlaySound(TEXT(_soundName), NULL, SND_ASYNC | SND_SYSTEM);
	}
}

