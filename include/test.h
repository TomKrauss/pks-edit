/*
 * test.h
 *
 * scanner definitions
 *
 * PROJECT: PKSEDIT/Command Editor
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * author: Tom
 * created: 03.02.1991
 */

# ifndef TEST_H
#define TEST_H

#define CT_EQ			0x01
#define CT_NE			0x02
#define CT_LT			0x03
#define CT_GT			0x04
#define CT_LE			0x05
#define CT_GE			0x06
#define CT_MATCH		0x07
#define CT_NMATCH		0x08

#define CT_NOT			0x21

#define CT_BRACKETS		0x43

#define CT_IS_UNARY(opcode)		(opcode == CT_NOT)
#define CT_IS_LOGICAL(opcode)	(opcode == CT_NOT)

extern char *decompile_stringForTestOperator(unsigned char testop);

# endif	/* TEST_H */

