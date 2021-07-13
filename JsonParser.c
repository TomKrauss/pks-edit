/*
 * JsonParser.c
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

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include "tos.h"
#include "linkedlist.h"
#include "arraylist.h"
#include "jsonparser.h"
#define JSMN_PARENT_LINKS
#include "jsmn.h"
#include "fileutil.h"
#include "stringutil.h"
#include "lineoperations.h"

#define MAX_TOKEN_SIZE		255

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
static void json_tokenContents(const char* json, jsmntok_t* tok, char* pDest) {
	size_t nSize = tok->end - tok->start;
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
	size_t tokenSize = tok->end - tok->start;
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
	return NULL;
}

/*
 * Convert a color specification from the JSON file. 
 */
static long json_convertColor(char* pszString) {
	if (pszString[0] == '#') {
		long l = string_convertToLongBase16(pszString + 1);
		return RGB((l >> 16) & 0xFF, (l >> 8) & 0xFF, l & 0xFF);
	}
	if (strcmp("white", pszString) == 0) {
		return RGB(255, 255, 255);
	}
	if (strcmp("red", pszString) == 0) {
		return RGB(255, 0, 0);
	}
	if (strcmp("green", pszString) == 0) {
		return RGB(0, 255, 0);
	}
	if (strcmp("blue", pszString) == 0) {
		return RGB(0, 0, 255);
	}
	return RGB(0, 0, 0);
}

/*
 * Returns an object array. Returns the index to the token array containing all tokens "after the array definition".
 */
static int json_getObjectList(char* pszBuf, jsmntok_t* tokens, int firstToken, int numberOfTokens, int bEnd, void** pTargetSlot, 
		struct tagARRAY_OBJECT_DESCRIPTOR* pArrayDescriptor) {
	int i = firstToken;

	while (i < numberOfTokens) {
		if (tokens[i].start > bEnd) {
			return i;
		}
		if (tokens[i].type == JSMN_OBJECT) {
			void* pNested = pArrayDescriptor->ro_createInstance();
			i = json_processTokens(pArrayDescriptor->ro_nestedRules, pNested, pszBuf, tokens[i].start, tokens[i].end, tokens, i + 1, numberOfTokens);
			ll_add((LINKED_LIST**)pTargetSlot, pNested);
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
				if (--maxElements <= 0) {
					break;
				}
				long i = string_convertToLong(tokenContents);
				*pTargetSlot++ = i;
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
static int json_getStringArray(char* pszBuf, jsmntok_t* tokens, int firstToken, int numberOfTokens, int bEnd, void** pTargetSlot, size_t maxElements) {
	char tokenContents[MAX_TOKEN_SIZE + 1];
	int i = firstToken;

	for (; i < numberOfTokens; i++) {
		if (tokens[i].start > bEnd) {
			break;
		}
		if (tokens[i].type == JSMN_STRING) {
			json_tokenContents(pszBuf, &tokens[i], tokenContents);
			if (*pTargetSlot == 0) {
				*pTargetSlot = arraylist_create(5);
			}
			arraylist_add((ARRAY_LIST*)*pTargetSlot, stralloc(tokenContents));
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
					*((char**)pTargetSlot) = string_allocate(tokenContents);
				}
				break;
			case RT_CHAR: 
				if (tokens[i].type == JSMN_STRING && tokenContents[1] == 0) {
					*((char*)pTargetSlot) = tokenContents[0];
				} else {
					long i = string_convertToLong(tokenContents);
					*((char*)pTargetSlot) = (char)i;
				}
				break;
			case RT_OBJECT_LIST:
				if (tokens[i].type == JSMN_ARRAY) {
					i = json_getObjectList(pszBuf, tokens, i + 1, numberOfTokens, tokens[i].end, pTargetSlot, &pRule->r_descriptor.r_t_arrayDescriptor)-1;
				}
				break;
			case RT_INTEGER_ARRAY:
				if (tokens[i].type == JSMN_ARRAY) {
					i = json_getIntArray(pszBuf, tokens, i+1, numberOfTokens, tokens[i].end, pTargetSlot, pRule->r_descriptor.r_t_maxElements)-1;
				}
				break;
			case RT_STRING_ARRAY:
				if (tokens[i].type == JSMN_ARRAY) {
					i = json_getStringArray(pszBuf, tokens, i + 1, numberOfTokens, tokens[i].end, pTargetSlot, pRule->r_descriptor.r_t_maxElements) - 1;
				}
				break;
			case RT_INTEGER: {
					long i = string_convertToLong(tokenContents);
					*((int*)pTargetSlot) = i;
				}
				break;
			case RT_SHORT: {
					long i = string_convertToLong(tokenContents);
					*((short*)pTargetSlot) = (short)i;
				}
				break;
			case RT_COLOR: {
					long i = json_convertColor(tokenContents);
					*((long*)pTargetSlot) = i;
				}
				break;
			case RT_FLAG:
				if (strcmp("true", tokenContents) == 0) {
					int bit = pRule->r_descriptor.r_t_flag;
					if (bit == 0) {
						bit = 1;
					}
					(*(int*)pTargetSlot) |= bit;
				}
				break;
			}
		}
	}
	return i;
}


/*
 * Parse the given JSON file and fill the target object according to the defined mapping rules. 
 */
int json_parse(char* pszFilename, void* pTargetObject, JSON_MAPPING_RULE* pRules) {
	char*	fn;
	int		fd;
	int		tokcount = 1000;
	char*	pszBuf;
	jsmn_parser parser;
	jsmntok_t* tokens;

	if ((fn = file_searchFileInPKSEditLocation(pszFilename)) != 0L && (fd = Fopen(fn, 0)) > 0) {
		jsmn_init(&parser);
		tokens = malloc(sizeof(*tokens) * tokcount);
		pszBuf = file_readFileAsString(fd);
		Fclose(fd);
		if (pszBuf) {
			int numberOfTokens = 0;
			size_t maxInputSize = strlen(pszBuf);
			while (tokens != NULL && (numberOfTokens = jsmn_parse(&parser, pszBuf, maxInputSize, tokens, tokcount)) == JSMN_ERROR_NOMEM) {
				tokcount *= 2;
				tokens = realloc(tokens, sizeof(*tokens) * tokcount);
			}
			json_processTokens(pRules, pTargetObject, pszBuf, 0, maxInputSize, tokens, 0, numberOfTokens);
			if (tokens != NULL) {
				free(tokens);
			}
			free(pszBuf);
		}
		return 1;
	} else {
		return 0;
	}
}

/*
 * Generate a JSON possibly quoted string from a source string.
 */
static void json_quote(char* pTarget, char* pSource) {
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

/*
 * Marshal a nested list of objects to the output stream.
 */
static void json_marshalObjectList(FILE* fp, int indent, void* pSourceObject, struct tagARRAY_OBJECT_DESCRIPTOR* pArrayDescriptor) {
	int bNeedsTermination;
	int bFirst = 1;
	JSON_MAPPING_RULE* pRules;

	while (pSourceObject) {
		if (bFirst) {
			bFirst = 0;
		} else {
			fprintf(fp, ",%s", _nl);
		}
		fprintf(fp, "%*c{%s", indent, ' ', _nl);
		pRules = pArrayDescriptor->ro_nestedRules;
		bNeedsTermination = 0;
		while (pRules->r_type != RT_END) {
			if (json_marshalNode(fp, indent+4, pSourceObject, pRules, bNeedsTermination)) {
				bNeedsTermination = 1;
			}
			pRules++;
		}
		fprintf(fp, "%s%*c}", _nl, indent, ' ');
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

/**
 * Write out a single node. 
 */
static int json_marshalNode(FILE* fp, int indent, void* pSourceObject, JSON_MAPPING_RULE* pRule, int bNeedsTermination) {
	void * pSourceSlot = ((char*)pSourceObject + pRule->r_targetOffset);
	char	tokenContents[MAX_TOKEN_SIZE + 1];
	char b[64];
	int bit;

	switch (pRule->r_type) {
	case RT_ALLOC_STRING:
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
	case RT_INTEGER:
		sprintf(tokenContents, "%d", *((int*)pSourceSlot));
		break;
	case RT_OBJECT_LIST:
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
	case RT_COLOR: {
		long rgb = *(long*)pSourceSlot;
		sprintf(tokenContents, "\"#%02x%02x%02x\"", GetRValue(rgb), GetGValue(rgb), GetBValue(rgb));
		}
		break;
	case RT_CHAR: {
			b[0] = (char) *((int*)pSourceSlot);
			if (!b[0]) {
				return 0;
			}
			b[1] = 0;
			json_quote(tokenContents, b);
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
int json_marshal(char* pszFilename, void* pSourceObject, JSON_MAPPING_RULE* pRules) {
	char* fn;
	FILE* fp;
	int indent = 0;
	int bNeedsTermination = 0;

	if ((fn = file_searchFileInPKSEditLocation(pszFilename)) != 0L && (fp = fopen(fn, "w")) != NULL) {
		fprintf(fp, "{%s", _nl);
		indent += 4;
		while (pRules->r_type != RT_END) {
			if (json_marshalNode(fp, indent, pSourceObject, pRules, bNeedsTermination)) {
				bNeedsTermination = 1;
			}
			pRules++;
		}
		fprintf(fp, "}%s", _nl);
		fclose(fp);
		return 1;
	}
	return 0;
}
