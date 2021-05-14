/*
 * SoundUtil.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: ring the bell
 *
 * 										created      : 
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauss
 *
 * 								Author: TOM
 */

#include <windows.h>
#include "lineoperations.h"

#define	CHANNEL		1
#define	PITCH		0			/* pitch + note == height */
#define	QUANTIZE		64

extern long Atol();
extern char *_strtolend;

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
		if (GetPksProfileString("bell",name,
							string,sizeof string) == 0)
			break;
		np = &_v.v_notes[i];
		np->n_note = Atol(string);
		np->n_duration = Atol(_strtolend);
		np->n_mode = Atol(_strtolend);
		np->n_volume = Atol(_strtolend);
	}
	if (i) {
		_v.v_nnotes = i;
	}
}

/*--------------------------------------------------------------------------
 * chime()
 */
EXPORT void chime(void)
{

	ReadChimeParams();

	if (!_v.v_nnotes) {
		MessageBeep(-1);
		return;
	}

#if !defined(WIN32)
	int		n;
	if (OpenSound() < 0) {
		MessageBeep(-1);
		return;
	}

	for (n = 0; n < _v.v_nnotes; n++) {
		np = &_v.v_notes[n];
		if (n == 0 || 
		   (np-1)->n_mode != np->n_mode ||
		   (np-1)->n_volume != np->n_volume) {
			SetVoiceAccent(_v.v_channel, _v.v_tempo, 
						np->n_volume, np->n_mode, PITCH);
		}
		if (np->n_duration &&
		    SetVoiceNote(_v.v_channel, np->n_note, 
					  QUANTIZE / np->n_duration, 1) != 0) {
			MessageBeep(0);
			CloseSound();
			return;
		}
	}

	StartSound();
	WaitSoundState(S_QUEUEEMPTY);
	StopSound();

	CloseSound();
#endif
}

