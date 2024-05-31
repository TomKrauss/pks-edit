/*
 * jsonparser.h
 *
 * Project: PKS Edit for Windows
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
	RT_FLOAT,				// we expect a "float" value in the JSON input.
	RT_INTEGER_ARRAY,		// we expect an array of int values in the JSON input and will assume the target object to contain an int[] data structure to fill.
	RT_SET,					// we expect a list of String values in the JSON input and will assume the target object to contain an 
							// hashmap of strings mapping to true or false to fill.
							// note, that the strings and array list are allocated and must be possibly freed later.
	RT_SHORT,				// we expect a "short" value in the JSON input.
	RT_ENUM,				// map JSON strings to int values.
	RT_COLOR,				// we expect a color specification (e.g.: #EEFF00 or red, green, blue).
	RT_OBJECT_LIST,			// a nested array of objects. Mapped to a linked list of objects. The r_descriptor contains a pointer to the mapping rules.
	RT_STRING_CALLBACK,		// the property is either a single string or an array of strings. The r_descriptr has a reference to a method to invoke
							// with the string. In case of an array, the method is invoked multiple times for every string in the array. If the method
							// returns 0, the calling is aborted.
	RT_NESTED_OBJECT,		// a nested single object. Mapped to sub-structure of the object created. The r_descriptor contains a pointer to the mapping rules.
	RT_STRING_ARRAY,		// we expect an array of String values in the JSON input and will assume the target object to be an ARRAY_LIST holding the Strings.
	RT_END					// the last rule in a list of rules must have this type.
} JSON_RULE_TYPE;

typedef void* (*FACTORY)();

struct tagOBJECT_DESCRIPTOR {
	FACTORY						 ro_createInstance;		// Factory creating the nested object
	struct tagJSON_MAPPING_RULE* ro_nestedRules;		// The rules describing the nested object
	size_t						 ro_nestedSize;			// If no factory is specified, one may specify the size of the objects contained in the array, which is then calloced.
	void (*ro_destroy)(void* p);						// An optional destruction method, invoked, when one uses json_destroy to de-allocate a nested JSON data structure.
};

typedef struct tagJSON_MAPPING_RULE {
	JSON_RULE_TYPE r_type;										// the type of the rule
	char* r_name;												// the name of the JSON value
	size_t r_targetOffset;										// the offset in the target object to which the mapping is performed.
	union tagNESTED_JSON_DESCRIPTOR {
		int		r_t_flag;										// if the rule type is RT_FLAG, this is the bit value (flag) associated with the value read.
		size_t  r_t_maxChars;									// the maximum number of bytes for RT_CHAR_ARRAY type json values.
		size_t	r_t_maxElements;								// maximum number of elements that fit into RT_INTEGER_ARRAY type data structures.
		JSON_STRING_CALLBACK r_t_callback;						// the string callback for type RT_STRING_CALLBACK
		const char** r_t_enumNames;								// NULL terminated array with the names corresponding to enum values: e.g.: {"small","medium","large",0}
		struct tagOBJECT_DESCRIPTOR r_t_arrayDescriptor;		// currently only used in case of type == RT_OBJECT_ARRAY
		struct tagJSON_MAPPING_RULE* r_t_nestedObjectRules;		// currently used in case of type == RT_NESTED_OBJECT. On a long term run, this should be identically to above and nested objects
																// should be allocated and destroyed as well.
	} r_descriptor;
} JSON_MAPPING_RULE;

/*
 * Parse the given JSON file and fill the target object according to the defined mapping rules.
 * The file name may be specified as an absolute path in which case it is used as is, or it might
 * be the base file name of a file to be loaded from the PKS Edit system pathes ($appDir/pks_sys, $user.hom/pks_sys).
 * In the later version, the parser may load multiple files allowing to extend settings.
 */
extern int json_parse(const char* pszFilename, BOOL extensionSupported, void* pTargetObject, JSON_MAPPING_RULE* pRules);

/*
 * Convert a (CSS) color specification from the JSON file.
 */
extern COLORREF json_convertColor(char* pszString);

/*
 * Write out an object with a given set of mapping rules in JSON format.
 */
int json_marshal(const char* pszFilename, void* pSourceObject, JSON_MAPPING_RULE* pRules);

/*
 * Generic method to release the memory occupied by an object described by mapping pRules read by the JSON reader.
 * Does not free the memory pointed to by pMemory itself.
 */
void json_destroy(void* pMemory, JSON_MAPPING_RULE* pRules);

#define JSONPARSER_H
#endif // !JSONPARSER_H

