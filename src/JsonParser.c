/*
 * JsonParser.c
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

#include <string.h>
#include "alloc.h"
#include <ctype.h>
#include <stdio.h>
#include "tos.h"
#include "linkedlist.h"
#include "arraylist.h"
#include "hashmap.h"
#include "jsonparser.h"
#define JSMN_PARENT_LINKS
#include "jsmn.h"
#include "fileutil.h"
#include "stringutil.h"
#include "documentmodel.h"
#include "pathname.h"
#include "trace.h"

static char* _nl = "\n";

/**
 * forward declaration. 
 */
static int json_processTokens(JSON_MAPPING_RULE* pRules, void* pTargetObject, char* pszBuf, int bStart, size_t bEnd, jsmntok_t* tokens,
	int firstToken, int numberOfTokens);
static int json_marshalNode(FILE* fp, int indent, void* pSourceObject, JSON_MAPPING_RULE* pRule, int bNeedsTermination);

/*
 * Get the actual contents of a JSON token as a zero terminated String. The string
 * must not exceed MAX_TOKEN_SIZE in size.
 */
void json_tokenContents(const char* json, jsmntok_t* tok, char* pDest) {
	size_t nSize = (size_t) tok->end - tok->start;
	if (nSize > MAX_TOKEN_SIZE) {
		nSize = MAX_TOKEN_SIZE;
	}
	for (int i = 0; i < nSize; i++) {
		char c = json[tok->start+i];
		if (c == '\\' && ++i < nSize) {
			c = json[tok->start + i];
			if (c == 'n') {
				*pDest++ = '\n';
				continue;
			} else if (c == 'f') {
				*pDest++ = '\f';
				continue;
			} else if (c == 'b') {
				*pDest++ = '\b';
				continue;
			} else if (c == 'r') {
				*pDest++ = '\r';
				continue;
			} else if (c == 't') {
				*pDest++ = '\t';
				continue;
			}
		}
		*pDest++ = c;
	}
	*pDest = 0;
}

/*
 * Checks, whether a JSON mapping rule matches a token. 
 */
static int json_matches(const char* json, jsmntok_t* tok, JSON_MAPPING_RULE* pRule) {
	jsmntype_t tokenType = JSMN_STRING;
	size_t tokenSize = (size_t) tok->end - tok->start;
	if (tok->type == tokenType && (int)strlen(pRule->r_name) == tokenSize &&
		strncmp(json + tok->start, pRule->r_name, tokenSize) == 0) {
		return 1;
	}
	return 0;
}

/*
 * Find a mapping rule for a JSON token input. 
 */
static JSON_MAPPING_RULE* json_findRule(char* pJsonInput, jsmntok_t* pToken, JSON_MAPPING_RULE* pRule) {
	while (pRule->r_type != RT_END) {
		if (json_matches(pJsonInput, pToken, pRule)) {
			return pRule;
		}
		pRule++;
	}
	char szBuf[256];
	size_t nLen = (size_t) pToken->end - pToken->start;
	memcpy(szBuf, pJsonInput + pToken->start, nLen);
	szBuf[nLen] = 0;

	return NULL;
}

static struct tagCSS_COLOR {
	const char* cc_name;
	COLORREF    cc_color;
} _cssColors[] = {
	{"white", RGB(255,255,255)},
	{"red", RGB(255,0,0)},
	{"green", RGB(0,128,0)},
	{"blue", RGB(0,0,255)},
	{"silver", RGB(192,192,192)},
	{"gray", RGB(128,128,128)},
	{"yellow", RGB(255,255,0)},
	{"maroon", RGB(128,0,0)},
	{"orange", RGB(230,145,56)},
	{"silver", RGB(192,192,192)},
	{"crimson", RGB(220,20,60)},
	{"brown", RGB(165,42,42)},
	{"coral", RGB(255,127,80)},
	{"cyan", RGB(0,255,255)},
	{"lime", RGB(0,255,0)},
	{"navy", RGB(0,0,128)},
	{"aqua", RGB(0,255,255)},
	{"teal", RGB(0,128,128)},
	{"fuchsia", RGB(255,0,255)},
	{"purple", RGB(128,0,128)}
};

/*
 * Convert a (CSS) color specification from the JSON file. 
 */
COLORREF json_convertColor(char* pszString) {
	if (pszString[0] == '#') {
		long l = (long)string_convertToLongBase16(pszString + 1);
		return RGB((l >> 16) & 0xFF, (l >> 8) & 0xFF, l & 0xFF);
	}
	for (int i = 0; i < DIM(_cssColors); i++) {
		if (strcmp(_cssColors[i].cc_name, pszString) == 0) {
			return _cssColors[i].cc_color;
		}
	}
	EdTRACE(log_message(DEBUG_WARN, "Cannot convert color %s from config file", pszString));
	return RGB(0, 0, 0);
}

/*
 * Returns an object array. Returns the index to the token array containing all tokens "after the array definition".
 */
static int json_getObjectList(char* pszBuf, jsmntok_t* tokens, int firstToken, int numberOfTokens, int bEnd, void** pTargetSlot, 
		struct tagOBJECT_DESCRIPTOR* pArrayDescriptor) {
	int i = firstToken;

	while (i < numberOfTokens) {
		if (tokens[i].start > bEnd) {
			return i;
		}
		if (tokens[i].type == JSMN_OBJECT) {
			void* pNested = pArrayDescriptor->ro_createInstance ? pArrayDescriptor->ro_createInstance() : calloc(1, pArrayDescriptor->ro_nestedSize);
			if (pNested == NULL) {
				break;
			}
			i = json_processTokens(pArrayDescriptor->ro_nestedRules, pNested, pszBuf, tokens[i].start, tokens[i].end, tokens, i + 1, numberOfTokens);
			ll_add(pTargetSlot, pNested);
		}
		else {
			i++;
		}
	}
	return i;
}

/*
 * Returns an int array. Returns the index to the token array containing all tokens "after the array definition".
 */
static int json_getIntArray(char* pszBuf, jsmntok_t* tokens, int firstToken, int numberOfTokens, int bEnd, int* pTargetSlot, size_t maxElements) {
	char tokenContents[MAX_TOKEN_SIZE + 1];
	int i = firstToken;

	for (; i < numberOfTokens; i++) {
		if (tokens[i].start > bEnd) {
			break;
		}
		if (tokens[i].type == JSMN_PRIMITIVE) {
			json_tokenContents(pszBuf, &tokens[i], tokenContents);
			if (isdigit(tokenContents[0])) {
				if (--maxElements == 0) {
					break;
				}
				long nLong = (long)string_convertToLong(tokenContents);
				*pTargetSlot++ = nLong;
			}
		}
	}
	// add terminator 0
	*pTargetSlot = 0;
	return i;
}

/*
 * Returns a String array_list. Returns the index to the token array containing all tokens "after the array definition".
 */
static int json_getStringArray(char* pszBuf, jsmntok_t* tokens, int firstToken, int numberOfTokens, int bEnd, ARRAY_LIST** pTargetSlot) {
	char tokenContents[MAX_TOKEN_SIZE + 1];
	int i = firstToken;
	ARRAY_LIST* pResult = arraylist_create(13);

	for (; i < numberOfTokens; i++) {
		if (tokens[i].start > bEnd) {
			break;
		}
		if (tokens[i].type == JSMN_STRING) {
			json_tokenContents(pszBuf, &tokens[i], tokenContents);
			arraylist_add(pResult, _strdup(tokenContents));
		}
	}
	*pTargetSlot = pResult;
	return i;
}

/*
 * Returns a String array_list. Returns the index to the token array containing all tokens "after the array definition".
 */
static int json_getSet(char* pszBuf, jsmntok_t* tokens, int firstToken, int numberOfTokens, int bEnd, void** pTargetSlot, size_t maxElements) {
	char tokenContents[MAX_TOKEN_SIZE + 1];
	int i = firstToken;

	for (; i < numberOfTokens; i++) {
		if (tokens[i].start > bEnd) {
			break;
		}
		if (tokens[i].type == JSMN_STRING) {
			json_tokenContents(pszBuf, &tokens[i], tokenContents);
			if (*pTargetSlot == 0) {
				*pTargetSlot = hashmap_create(19, NULL, NULL);
			}
			char* pszKey = _strdup(tokenContents);
			if (!hashmap_put((HASHMAP*)*pTargetSlot, pszKey, 1)) {
				free(pszKey);
			}
		}
	}
	return i;
}

/*
 * Process the tokens found in a JSON object defining the values of the pTargetObject object. 
 */
static int json_processTokens(JSON_MAPPING_RULE* pRules, void* pTargetObject, char* pszBuf, int bStart, size_t bEnd, jsmntok_t* tokens, 
		int firstToken, int numberOfTokens) {
	char	tokenContents[MAX_TOKEN_SIZE + 1];
	int i = firstToken;
	for (; i < numberOfTokens; i++) {
		if (tokens[i].start > bEnd) {
			break;
		}
		if (tokens[i].type != JSMN_STRING) {
			continue;
		}
		JSON_MAPPING_RULE* pRule = json_findRule(pszBuf, &tokens[i], pRules);
		i++;
		if (pRule != NULL) {
			void* pTargetSlot;
			pTargetSlot = ((char*)pTargetObject + pRule->r_targetOffset);
			json_tokenContents(pszBuf, &tokens[i], tokenContents);
			switch (pRule->r_type) {
			case RT_STRING_CALLBACK: {
					if (tokens[i].type == JSMN_STRING) {
						pRule->r_descriptor.r_t_callback(pTargetObject, tokenContents);
					} else if (tokens[i].type == JSMN_ARRAY) {
						int nArrayEnd = tokens[i].end;
						i++;
						BOOL bCall = TRUE;
						while (i < numberOfTokens) {
							if (tokens[i].end > nArrayEnd) {
								i--;
								break;
							}
							if (tokens[i].type == JSMN_STRING && bCall) {
								json_tokenContents(pszBuf, &tokens[i], tokenContents);
								bCall = pRule->r_descriptor.r_t_callback(pTargetObject, tokenContents);
							}
							i++;
						}
					}
				}
				break;
			case RT_ENUM:
				if (tokens[i].type == JSMN_STRING) {
					size_t nLength = strlen(tokenContents);
					for (int nStrIndex = 0; pRule->r_descriptor.r_t_enumNames[nStrIndex]; nStrIndex++) {
						if (strncmp(tokenContents, pRule->r_descriptor.r_t_enumNames[nStrIndex], nLength) == 0) {
							*((int*)pTargetSlot) = nStrIndex;
							break;
						}
					}
				}
				break;
			case RT_CHAR_ARRAY:
				if (tokens[i].type == JSMN_STRING) {
					size_t nLength = strlen(tokenContents);
					if (nLength >= pRule->r_descriptor.r_t_maxChars-1) {
						nLength = pRule->r_descriptor.r_t_maxChars - 1;
					}
					strncpy(pTargetSlot, tokenContents, nLength);
					((char*)pTargetSlot)[nLength] = 0;
				}
				break;
			case RT_ALLOC_STRING:
				if (tokens[i].type == JSMN_STRING) {
					*((char**)pTargetSlot) = _strdup(tokenContents);
				}
				break;
			case RT_CHAR_ZERO:
			case RT_CHAR: 
				if (tokens[i].type == JSMN_STRING && tokenContents[1] == 0) {
					*((char*)pTargetSlot) = tokenContents[0];
				} else {
					long nChar = (long)string_convertToLong(tokenContents);
					*((char*)pTargetSlot) = (char)nChar;
				}
				break;
			case RT_OBJECT_LIST:
				if (tokens[i].type == JSMN_ARRAY) {
					i = json_getObjectList(pszBuf, tokens, i + 1, numberOfTokens, tokens[i].end, pTargetSlot, &pRule->r_descriptor.r_t_arrayDescriptor) - 1;
				}
				break;
			case RT_NESTED_OBJECT:
				if (tokens[i].type == JSMN_OBJECT) {
					void* pNested = pTargetSlot;
					i = json_processTokens(pRule->r_descriptor.r_t_nestedObjectRules, pNested, pszBuf, tokens[i].start, tokens[i].end, tokens, i + 1, numberOfTokens)-1;
				}
				break;
			case RT_INTEGER_ARRAY:
				if (tokens[i].type == JSMN_ARRAY) {
					i = json_getIntArray(pszBuf, tokens, i+1, numberOfTokens, tokens[i].end, pTargetSlot, pRule->r_descriptor.r_t_maxElements)-1;
				}
				break;
			case RT_STRING_ARRAY:
				if (tokens[i].type == JSMN_ARRAY) {
					i = json_getStringArray(pszBuf, tokens, i + 1, numberOfTokens, tokens[i].end, pTargetSlot) - 1;
				}
				break;
			case RT_SET:
				if (tokens[i].type == JSMN_ARRAY) {
					i = json_getSet(pszBuf, tokens, i + 1, numberOfTokens, tokens[i].end, pTargetSlot, pRule->r_descriptor.r_t_maxElements) - 1;
				}
				break;
			case RT_INTEGER: {
					long nInt = (long)string_convertToLong(tokenContents);
					*((int*)pTargetSlot) = nInt;
				}
				break;
			case RT_FLOAT: {
					float nFloat = (float)atof(tokenContents);
					*((float*)pTargetSlot) = nFloat;
				}
				break;
			case RT_SHORT: {
					long nInt = (long)string_convertToLong(tokenContents);
					*((short*)pTargetSlot) = (short)nInt;
				}
				break;
			case RT_COLOR: {
					long nInt = json_convertColor(tokenContents);
					*((long*)pTargetSlot) = nInt;
				}
				break;
			case RT_FLAG: {
				int bit = pRule->r_descriptor.r_t_flag;
				if (bit == 0) {
					bit = 1;
				}
				if (strcmp("true", tokenContents) == 0) {
					(*(int*)pTargetSlot) |= bit;
				}
				else if (strcmp("false", tokenContents) == 0) {
					(*(int*)pTargetSlot) &= ~bit;
				}
				break;
			}
			}
		}
	}
	return i;
}

/*
 * Parse a memory space or characters with a given maxInputSize and return the JSON tokens 
 * and the number of tokens in pNTokens.
 */
jsmntok_t* json_parseMemory(char* pszBuf, size_t maxInputSize, int* pNTokens) {
	int	tokcount = 2048;
	jsmn_parser parser;
	jsmn_init(&parser);
	jsmntok_t* tokens = malloc(sizeof(*tokens) * tokcount);
	int numberOfTokens = 0;
	while (tokens != NULL && (numberOfTokens = jsmn_parse(&parser, pszBuf, maxInputSize, tokens, tokcount)) == JSMN_ERROR_NOMEM) {
		tokcount *= 2;
		jsmntok_t* pszTempTokens = realloc(tokens, sizeof(*tokens) * tokcount);
		if (pszTempTokens == NULL) {
			free(tokens);
			free(pszBuf);
			return 0;
		}
		tokens = pszTempTokens;
	}
	*pNTokens = numberOfTokens;
	return tokens;
}

/*
 * Parse a JSON file from the file descriptor according to mapping rules passed as an argument
 * into the target object pTargetObject.
 */
static int json_parseFile(HFILE fd, void* pTargetObject, JSON_MAPPING_RULE* pRules) {
	char* pszBuf;
	pszBuf = file_readFileAsString(fd);
	if (pszBuf) {
		size_t maxInputSize = strlen(pszBuf);
		int numberOfTokens = 0;
		jsmntok_t* tokens = json_parseMemory(pszBuf, maxInputSize, &numberOfTokens);
		json_processTokens(pRules, pTargetObject, pszBuf, 0, maxInputSize, tokens, 0, numberOfTokens);
		free(tokens);
		free(pszBuf);
	}
	return 1;
}

/*
 * Parse the given JSON file and fill the target object according to the defined mapping rules. 
 */
int json_parse(const char* pszFilename, BOOL extensionSupported, void* pTargetObject, JSON_MAPPING_RULE* pRules) {
	char*	fn;
	int     fd;
	int		ret = 0;

	if ((fn = file_searchFileInPKSEditLocationFlags(pszFilename, 
				CFSF_SEARCH_ABSOLUTE | CFSF_SEARCH_APP_PKS_SYS| CFSF_SEARCH_LOCAL_PKS_SYS| CFSF_SEARCH_PKS_SYS_OVERRIDE_DIR)) != 0L
			&& (fd = file_openFile(fn, FALSE)) > 0) {
		EdTRACE(log_message(DEBUG_INFO, "Loading config file %s", fn));
		ret = json_parseFile(fd, pTargetObject, pRules);
		file_closeFile(&fd);
	}
	// Allow extending configurations by placing config files into user.home/CONFIG
	if (extensionSupported && 
		(fn = file_searchFileInPKSEditLocationFlags(pszFilename, CFSF_SEARCH_PKS_SYS_EXTENSION_DIR)) != 0L && 
		(fd = file_openFile(fn, FALSE)) > 0) {
		EdTRACE(log_message(DEBUG_INFO, "Loading extension file %s", fn));
		json_parseFile(fd, pTargetObject, pRules);
		file_closeFile(&fd);
	}
	return ret;
}

/*
 * Generate a JSON possibly quoted string from a source string.
 */
static void json_quote(char* pTarget, const char* pSource) {
	char* pEnd = pTarget + MAX_TOKEN_SIZE-2;
	*pTarget++ = '"';
	while (pTarget < pEnd) {
		char c = *pSource++;
		if (c == 0) {
			break;
		}
		if (c == '\t') {
			*pTarget++ = '\\';
			c = 't';
		} else if (c == '\r') {
			*pTarget++ = '\\';
			c = 'r';
		} else if (c == '\n') {
			*pTarget++ = '\\';
			c = 'n';
		} else if (c == '\b') {
			*pTarget++ = '\\';
			c = 'b';
		} else if (c == '"' || c == '\\') {
			*pTarget++ = '\\';
		}
		*pTarget++ = c;
	}
	*pTarget++ = '"';
	*pTarget = 0;
}

static int json_marshalObject(FILE* fp, int indent, BOOL bFirstIndent, void* pSourceObject, JSON_MAPPING_RULE* pRules) {
	fprintf(fp, "%*c{%s", bFirstIndent ? indent : 0, ' ', _nl);
	int bNeedsTermination = 0;
	while (pRules->r_type != RT_END) {
		if (json_marshalNode(fp, indent + 4, pSourceObject, pRules, bNeedsTermination)) {
			bNeedsTermination = 1;
		}
		pRules++;
	}
	fprintf(fp, "%s%*c}", _nl, indent, ' ');
	return bNeedsTermination;
}

/*
 * Marshal a nested list of objects to the output stream.
 */
static void json_marshalObjectList(FILE* fp, int indent, void* pSourceObject, struct tagOBJECT_DESCRIPTOR* pArrayDescriptor) {
	int bFirst = 1;

	while (pSourceObject) {
		if (bFirst) {
			bFirst = 0;
		} else {
			fprintf(fp, ",%s", _nl);
		}
		json_marshalObject(fp, indent, TRUE, pSourceObject, pArrayDescriptor->ro_nestedRules);
		pSourceObject = ((LINKED_LIST*)pSourceObject)->next;
	}
	fprintf(fp, _nl);
}

/*
 * Marshal an int array to the output stream in the form a, b, c
 */
static void json_marshalIntArray(char* pszToken, int* pValues) {
	int bFirst = 1;
	char buf[10];
	while (*pValues) {
		if (bFirst) {
			bFirst = 0;
		} else {
			strcat(pszToken, ", ");
		}
		sprintf(buf, "%d", *pValues);
		strcat(pszToken, buf);
		pValues++;
	}
}

/*
 * Marshal a string array list to the output stream in the form "a", "b", "c"
 */
static void json_marshalStringArray(FILE* fp, int indent, ARRAY_LIST* pValues) {
	char	tokenContents[MAX_TOKEN_SIZE + 1];
	size_t nSize = arraylist_size(pValues);
	for (int i = 0; i < nSize; i++) {
		if (i > 0) {
			fprintf(fp, ",%s", _nl);
		}
		char* pszVal = arraylist_get(pValues, i);
		json_quote(tokenContents, pszVal);
		fprintf(fp, "%*c%s", indent, ' ', tokenContents);
	}
}

/**
 * Write out a single node. 
 */
static int json_marshalNode(FILE* fp, int indent, void* pSourceObject, JSON_MAPPING_RULE* pRule, int bNeedsTermination) {
	void * pSourceSlot = ((char*)pSourceObject + pRule->r_targetOffset);
	char	tokenContents[MAX_TOKEN_SIZE + 1];
	int bit;

	switch (pRule->r_type) {
	case RT_ALLOC_STRING:
		pSourceSlot = *((char**)pSourceSlot);
		if (!pSourceSlot) {
			return 0;
		}
	case RT_CHAR_ARRAY:
		if (!(*(char*)pSourceSlot)) {
			return 0;
		}
		json_quote(tokenContents, pSourceSlot);
		break;
	case RT_SHORT:
		sprintf(tokenContents, "%d", (int)*((short*)pSourceSlot));
		break;
	case RT_FLAG:
		bit = pRule->r_descriptor.r_t_flag;
		if ((bit == 0 && (*(int*)pSourceSlot)) || (bit != 0 && ((*(int*)pSourceSlot) & bit))) {
			strcpy(tokenContents, "true");
		} else {
			return 0;
		}
		break;
	case RT_ENUM: {
		int nVal = *((int*)pSourceSlot);
		if (nVal >= 0) {
			json_quote(tokenContents, pRule->r_descriptor.r_t_enumNames[nVal]);
		}
	}
	break;
	case RT_INTEGER:
		sprintf(tokenContents, "%d", *((int*)pSourceSlot));
		break;
	case RT_FLOAT:
		sprintf(tokenContents, "%f", *((float*)pSourceSlot));
		break;
	case RT_NESTED_OBJECT:
		if (bNeedsTermination) {
			fprintf(fp, ",%s", _nl);
		}
		fprintf(fp, "%*c\"%s\":", indent, ' ', pRule->r_name);
		json_marshalObject(fp, indent, FALSE, pSourceSlot, pRule->r_descriptor.r_t_nestedObjectRules);
		return 1;
	case RT_OBJECT_LIST:
		if (*(void**)pSourceSlot == NULL) {
			return 0;
		}
		if (bNeedsTermination) {
			fprintf(fp, ",%s", _nl);
		}
		fprintf(fp, "%*c\"%s\": [%s", indent, ' ', pRule->r_name, _nl);
		json_marshalObjectList(fp, indent, *(void**)pSourceSlot, &pRule->r_descriptor.r_t_arrayDescriptor);
		fprintf(fp, "%*c]", indent, ' ');
		return 1;
	case RT_INTEGER_ARRAY:
		if (!*((int*)pSourceSlot)) {
			return 0;
		}
		strcpy(tokenContents, "[");
		json_marshalIntArray(tokenContents, pSourceSlot);
		strcat(tokenContents, "]");
		break;
	case RT_STRING_ARRAY:
		if (!*((ARRAY_LIST**)pSourceSlot)) {
			return 0;
		}
		if (bNeedsTermination) {
			fprintf(fp, ",%s", _nl);
		}
		fprintf(fp, "%*c\"%s\": [%s", indent, ' ', pRule->r_name, _nl);
		json_marshalStringArray(fp, indent+4, *(ARRAY_LIST**)pSourceSlot);
		fprintf(fp, "%s%*c]", _nl, indent, ' ');
		return 1;
	case RT_COLOR: {
		long rgb = *(long*)pSourceSlot;
		sprintf(tokenContents, "\"#%02x%02x%02x\"", GetRValue(rgb), GetGValue(rgb), GetBValue(rgb));
		}
		break;
	case RT_CHAR_ZERO:
	case RT_CHAR: {
			char b[2] = { 0 };
			b[0] = (char) *((int*)pSourceSlot);
			if (!b[0]) {
				if (pRule->r_type == RT_CHAR_ZERO) {
					strcpy(tokenContents, "\"\\\\0\"");
				} else {
					return 0;
				}
			} else {
				json_quote(tokenContents, b);
			}
		}
		break;
	}
	if (bNeedsTermination) {
		fprintf(fp, ",%s", _nl);
	}
	fprintf(fp, "%*c\"%s\": %s", indent, ' ', pRule->r_name, tokenContents);
	return 1;
}

/*
 * Write out an object with a given set of mapping rules in JSON format.
 */
int json_marshal(const char* pszFilename, void* pSourceObject, JSON_MAPPING_RULE* pRules) {
	const char* fn;
	FILE* fp;
	int indent = 0;
	int bNeedsTermination = 0;
	fn = file_searchFileInPKSEditLocation(pszFilename);
	char filename[EDMAXPATHLEN];
	if (fn == NULL) {
		if (file_isAbsolutePathName(pszFilename)) {
			fn = pszFilename;
		} else {
			string_concatPathAndFilename(filename, _pksSysFolder, pszFilename);
			fn = filename;
		}
	}
	if ((fp = fopen(fn, "w")) != NULL) {
		EdTRACE(log_message(DEBUG_INFO, "Saving config file %s", fn));
		fprintf(fp, "{%s", _nl);
		indent += 4;
		while (pRules->r_type != RT_END) {
			if (json_marshalNode(fp, indent, pSourceObject, pRules, bNeedsTermination)) {
				bNeedsTermination = 1;
			}
			pRules++;
		}
		if (bNeedsTermination) {
			fprintf(fp, "%s", _nl);
		}
		fprintf(fp, "}%s", _nl);
		fclose(fp);
		return 1;
	}
	return 0;
}

static void json_destroyNode(void* pSourceObject, JSON_MAPPING_RULE* pRule);

static int json_destroyObject(void* pSourceObject, JSON_MAPPING_RULE* pRules) {
	while (pRules->r_type != RT_END) {
		json_destroyNode(pSourceObject, pRules);
		pRules++;
	}
	return 1;
}

static int json_destroyObjectIncl(void* pSourceObject, JSON_MAPPING_RULE* pRules) {
	json_destroyObject(pSourceObject, pRules);
	free(pSourceObject);
	return 1;
}

static void json_destroyNode(void* pSourceObject, JSON_MAPPING_RULE* pRule) {
	void* pSourceSlot = ((char*)pSourceObject + pRule->r_targetOffset);
	LINKED_LIST* pList;
	ARRAY_LIST* pArrayList;
	HASHMAP* pMap;

	switch (pRule->r_type) {
	case RT_ALLOC_STRING:
		pSourceSlot = *((char**)pSourceSlot);
		if (pSourceSlot) {
			free(pSourceSlot);
		}
		break;
	case RT_INTEGER_ARRAY:
	case RT_CHAR_ARRAY:
	case RT_SHORT:
	case RT_FLAG:
	case RT_COLOR:
	case RT_ENUM: 
	case RT_CHAR_ZERO:
	case RT_CHAR:
		break;
	case RT_NESTED_OBJECT:
		json_destroyObjectIncl(pSourceSlot, pRule->r_descriptor.r_t_nestedObjectRules);
		break;
	case RT_SET: 
		pMap = *(HASHMAP**)pSourceSlot;
		if (pMap == NULL) {
			break;
		}
		hashmap_destroySet(pMap);
		break;
	case RT_STRING_ARRAY:
		pArrayList = *(ARRAY_LIST**)pSourceSlot;
		if (pArrayList == NULL) {
			break;
		}
		arraylist_destroyStringList(pArrayList);
		break;
	case RT_OBJECT_LIST:
		pList = *(LINKED_LIST**)pSourceSlot;
		if (pList == NULL) {
			break;
		}
		ll_destroy2(&pList, json_destroyObject, pRule->r_descriptor.r_t_arrayDescriptor.ro_nestedRules);
		break;
	}
}

/*
 * Generic method to release the memory occupied by an object described by mapping pRules read by the JSON reader.
 */
void json_destroy(void* pMemory, JSON_MAPPING_RULE* pRules) {
	json_destroyObject(pMemory, pRules);
}
