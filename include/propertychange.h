/*
 * propertychange.h
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS
 *
 * purpose: property change handling.
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef PROPERTYCHANGE_H

union PROPERTY_CHANGE_VALUE {
	void* v_object;
	long  v_simpleValue;
	char* v_string;
};

typedef enum enumPROPERTY_CHANGE_TYPE { FT_FLAGS, FT_NAME } PROPERTY_CHANGE_TYPE;

typedef struct tagPROPERTY_CHANGE {
	void* prop_sender;								// the object in which the property has changed (the FTABLE or the like)
	PROPERTY_CHANGE_TYPE prop_type;					// type of change
	union PROPERTY_CHANGE_VALUE prop_oldValue;		// the previous value
	union PROPERTY_CHANGE_VALUE prop_newValue;		// the new value
} PROPERTY_CHANGE;

#define PROPERTYCHANGE_H
#endif // !PROPERTYCHANGE_H

