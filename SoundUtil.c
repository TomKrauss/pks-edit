/*
 * SoundUtil.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS
 *
 * purpose: ring the bell
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
#include "lineoperations.h"
#include "stringutil.h"
#include "editorconfiguration.h"

#define	CHANNEL		1
#define	PITCH		0			/* pitch + note == height */
#define	QUANTIZE		64

typedef struct tagNOTE {
	unsigned char n_note;
	unsigned char n_duration;		/* given in 1/64 */
	unsigned char n_mode;
	unsigned char n_volume;
} NOTE;

typedef struct tagVOICEQ {
	int		v_channel;
	int		v_tempo;
	int		v_nnotes;
	NOTE		*v_notes;
} VOICEQ;

static NOTE   _notes[16] = {22,	16,		S_NORMAL,		255};

static VOICEQ _v = { CHANNEL,	120, 	0,	_notes };

/*--------------------------------------------------------------------------
 * ReadChimeParams()
 */
static void ReadChimeParams()
{
	static int 	done;
	int			i;
	char			name[16],string[128];
	NOTE			*np;

	if (done++)
		return;

	for (i = 0; i < DIM(_notes); i++) {
		wsprintf(name,"N%d",i);
		if (prof_getPksProfileString("bell",name,
							string,sizeof string) == 0)
			break;
		np = &_v.v_notes[i];
		np->n_note = (unsigned char) string_convertToLong(string);
		np->n_duration = (unsigned char) string_convertToLong(_strtolend);
		np->n_mode = (unsigned char) string_convertToLong(_strtolend);
		np->n_volume = (unsigned char) string_convertToLong(_strtolend);
	}
	if (i) {
		_v.v_nnotes = i;
	}
}

/*--------------------------------------------------------------------------
 * sound_playChime()
 * Play a chime sound.
 */
EXPORT void sound_playChime(void)
{

	ReadChimeParams();

	if (!_v.v_nnotes) {
		MessageBeep(-1);
		return;
	}

}

