/*
 * propertychange.h
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: property change handling.
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * (c) Pahlen & Krauss
 *
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

