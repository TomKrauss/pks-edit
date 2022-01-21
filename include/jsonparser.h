/*
 * jsonparser.h
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: generic JSON parser mapping data structures
 * against JSON files.
 *
 * 										created: 2.7.2021
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef JSONPARSER_H

/*
 * Callback invoked, for RT_STRING_CALLBACK. 1st parameter is the target object constructed during parsing the JSON file,
 * 2nd param is the string found.
 */
typedef int (*JSON_STRING_CALLBACK)(void* pTargetObject, const char* pszString);

typedef enum { 
	RT_CHAR_ARRAY,			// we expect a string in the JSON input and will store the value in the target object. The value descriptor will contain the maximum size
	RT_ALLOC_STRING,		// we expect a string in the JSON input and will return a malloced version of the string in the target object 
	RT_CHAR,				// we expect a number in the JSON input, which will define a character value
	RT_FLAG,				// we expect a boolean value in the JSON input. If true, we will use the 
	RT_INTEGER,				// we expect an "int" value in the JSON input.
	RT_INTEGER_ARRAY,		// we expect an array of int values in the JSON input and will assume the target object to contain an int[] data structure to fill.
	RT_SET,					// we expect a list of String values in the JSON input and will assume the target object to contain an 
							// hashmap of strings mapping to true or false to fill.
							// note, that the strings and array list are allocated and must be possibly freed later.
	RT_SHORT,				// we expect a "short" value in the JSON input.
	RT_COLOR,				// we expect a color specification (e.g.: #EEFF00 or red, green, blue).
	RT_OBJECT_LIST,			// a nested array of objects. Mapped to a linked list of objects. The r_descriptor contains a pointer to the mapping rules.
	RT_STRING_CALLBACK,		// the property is either a single string or an array of strings. The r_descriptr has a reference to a method to invoke
							// with the string. In case of an array, the method is invoked multiple times for every string in the array. If the method
							// returns 0, the calling is aborted.
	RT_END					// the last rule in a list of rules must have this type.
} JSON_RULE_TYPE;

typedef void* (*FACTORY)();

struct tagARRAY_OBJECT_DESCRIPTOR {
	FACTORY						 ro_createInstance;		// Factory creating the nested object
	struct tagJSON_MAPPING_RULE* ro_nestedRules;		// The rules describing the nested object
};

typedef struct tagJSON_MAPPING_RULE {
	JSON_RULE_TYPE r_type;		// the type of the rule
	char* r_name;				// the name of the JSON value
	size_t r_targetOffset;		// the offset in the target object to which the mapping is performed.
	union tagNESTED_JSON_DESCRIPTOR {
		int		r_t_flag;		// if the rule type is RT_FLAG, this is the bit value (flag) associated with the value read.
		size_t  r_t_maxChars;	// the maximum number of bytes for RT_CHAR_ARRAY type json values.
		size_t	r_t_maxElements; // maximum number of elements that fit into RT_INTEGER_ARRAY type data structures.
		JSON_STRING_CALLBACK r_t_callback;	// the string callback for type RT_STRING_CALLBACK
		struct tagARRAY_OBJECT_DESCRIPTOR r_t_arrayDescriptor;		// in case of type == RT_OBJECT_ARRAY
	} r_descriptor;
} JSON_MAPPING_RULE;

/*
 * Parse the given JSON file and fill the target object according to the defined mapping rules.
 */
extern int json_parse(const char* pszFilename, void* pTargetObject, JSON_MAPPING_RULE* pRules);

/*
 * Write out an object with a given set of mapping rules in JSON format.
 */
int json_marshal(char* pszFilename, void* pSourceObject, JSON_MAPPING_RULE* pRules);

#define JSONPARSER_H
#endif // !JSONPARSER_H

