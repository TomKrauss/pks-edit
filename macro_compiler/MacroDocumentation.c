/*
 * MacroDocumentation.c
 *
 * Provide documentation for PKSMacroC source code.
 *fu
 * PROJECT: PKSEDIT
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * author : TOM
 * created: 12.05.2022
 */

#include <stdio.h>
#include <string.h>

#include "alloc.h"
#include "stringutil.h"
#include "pksmacrocvm.h"
#include "pksmacro.h"
#include "funcdef.h"
#include "symbols.h"

 /*
  * Protocol prefix for hyperlinks inside the help window.
  */
#define MACROREF_PROTOCOL				"macroref://"

static char* macrodoc_printTypeWithLink(const char* pszType) {
	static char szPrinted[256];
	PKS_TYPE_DESCRIPTOR* pDescriptor = types_getTypeDescriptor(pszType);
	if (!pDescriptor) {
		return (char*)pszType;
	}
	sprintf(szPrinted, "<a href=\"%s%s\">%s</a>", MACROREF_PROTOCOL, pszType, pszType);
	return szPrinted;
}

static const char* macrodoc_pksTypeFromParamtype(PARAMETER_TYPE_DESCRIPTOR pt) {
	if (pt.pt_type == PARAM_TYPE_STRING_ARRAY) {
		return "string[]";
	}
	return macrodoc_printTypeWithLink(types_nameFor(pt.pt_valueType));
}

static void macrodoc_startHelpSection(STRING_BUF* pBuf, const char* pszName) {
	stringbuf_appendString(pBuf, "<p><b>");
	stringbuf_appendString(pBuf, pszName);
	stringbuf_appendString(pBuf, "</b>: ");
}

/*
 * Formats a hyperlink reference to be added to the documentation. Will
 * destroy the 2nd parameter as a side effect.
 */
static void macrodoc_formatJavadocLink(STRING_BUF* pBuf, STRING_BUF* psbLink) {
	stringbuf_appendString(pBuf, "<a href=\"");
	stringbuf_appendString(pBuf, MACROREF_PROTOCOL);
	stringbuf_appendString(pBuf, stringbuf_getString(psbLink));
	stringbuf_appendString(pBuf, "\">");
	stringbuf_appendString(pBuf, stringbuf_getString(psbLink));
	stringbuf_appendString(pBuf, "</a>");
	stringbuf_destroy(psbLink);
}

/*
 * Format the comment of a PKSMacroC type/enum/macro/ ... documentation, which allows for
 * similar special syntax as documentation in Java code (Javadoc format).
 */
static void macrodoc_formatJavadocComment(STRING_BUF* pBuf, const char* pszInput) {
	char szTag[64];
	char* pszTag = 0;
	BOOL bParamsFound = 0;
	BOOL bHighlightWord = 0;
	char c;
	STRING_BUF* psbLink = 0;

	memset(szTag, 0, sizeof szTag);
	macrodoc_startHelpSection(pBuf, "Description");
	while ((c = *pszInput++) != 0) {
		if (!pszTag) {
			if (c == '@') {
				pszTag = szTag;
				bHighlightWord = 0;
			}
			else {
				if (psbLink) {
					if (c == '\n') {
						macrodoc_formatJavadocLink(pBuf, psbLink);
						psbLink = 0;
					}
					else if (!isspace((unsigned char)c)) {
						stringbuf_appendChar(psbLink, c);
					}
					continue;
				}
				if (bHighlightWord == 1 && isalpha((unsigned char)c)) {
					stringbuf_appendString(pBuf, "<em>");
					bHighlightWord = 2;
				}
				else if (bHighlightWord == 2 && !isalpha((unsigned char)c)) {
					stringbuf_appendString(pBuf, "</em> - ");
					bHighlightWord = 0;
				}
				stringbuf_appendChar(pBuf, c);
			}
		} else {
			if (!isalpha((unsigned char)c)) {
				*pszTag = 0;
				pszTag = 0;
				if (strcmp("param", szTag) == 0) {
					if (!bParamsFound) {
						stringbuf_appendString(pBuf, "</p>");
						macrodoc_startHelpSection(pBuf, "Parameters");
						bParamsFound = 1;
					}
					else {
						stringbuf_appendString(pBuf, "<br>");
					}
					bHighlightWord = 1;
				}
				else if (strcmp("see", szTag) == 0) {
					stringbuf_appendString(pBuf, "</p><b>See also:</b>&nbsp;");
					psbLink = stringbuf_create(64);
				}
				else {
					szTag[0] = toupper(szTag[0]);
					stringbuf_appendString(pBuf, "</p>");
					macrodoc_startHelpSection(pBuf, szTag);
				}
			}
			else {
				*pszTag++ = c;
			}
		}
	}
	if (psbLink) {
		macrodoc_formatJavadocLink(pBuf, psbLink);
	}
	stringbuf_appendString(pBuf, "</p>");
}

/*
 * Returns the pointer to an allocated string containing the HTML formatted
 * help for a native PKSMacroC function.
 */
const char* macrodoc_helpForNativeFunction(const char* pszName, NATIVE_FUNCTION* pFunc) {
	if (!pFunc->nf_description) {
		return 0;
	}
	const char* pszParameterDescription = pFunc->nf_parameters;
	STRING_BUF* pszParams = stringbuf_create(100);
	int nParams = function_getParameterCount(pFunc);
	for (int i = 1; i <= nParams || (pszParameterDescription && *pszParameterDescription); i++) {
		char szParamType[256];
		char szParamName[256];
		if (i > 1) {
			stringbuf_appendString(pszParams, ", ");
		}
		PARAMETER_TYPE_DESCRIPTOR pt = function_getParameterTypeDescriptor(pFunc, i);
		szParamType[0] = 0;
		szParamName[0] = 0;
		if (pszParameterDescription && *pszParameterDescription) {
			char* pszDest = szParamType;
			while (*pszParameterDescription) {
				char c = *pszParameterDescription++;
				if (c == ',') {
					break;
				}
				if (c == ' ') {
					*pszDest = 0;
					pszDest = szParamName;
					continue;
				}
				*pszDest++ = c;
			}
			*pszDest = 0;
		}
		if (!szParamType[0] || pt.pt_type == PARAM_TYPE_ENUM || pt.pt_type == PARAM_TYPE_BITSET) {
			strcpy(szParamType, (char*)macrodoc_pksTypeFromParamtype(pt));
		}
		else {
			strcpy(szParamType, macrodoc_printTypeWithLink(szParamType));
		}
		if (!szParamName[0]) {
			sprintf(szParamName, "p%d", i);
		}
		stringbuf_appendString(pszParams, szParamType);
		stringbuf_appendChar(pszParams, ' ');
		stringbuf_appendString(pszParams, szParamName);
	}
	STRING_BUF* pBuf = stringbuf_create(200);
	macrodoc_startHelpSection(pBuf, "Synopsis");
	stringbuf_appendString(pBuf, macrodoc_pksTypeFromParamtype(function_getParameterTypeDescriptor(pFunc, 0)));
	stringbuf_appendChar(pBuf, ' ');
	stringbuf_appendString(pBuf, pszName);
	stringbuf_appendChar(pBuf, '(');
	stringbuf_appendString(pBuf, stringbuf_getString(pszParams));
	stringbuf_destroy(pszParams);
	stringbuf_appendChar(pBuf, ')');
	stringbuf_appendString(pBuf, "</p>");

	macrodoc_formatJavadocComment(pBuf, pFunc->nf_description);
	char* pszRet = _strdup(stringbuf_getString(pBuf));
	stringbuf_destroy(pBuf);

	return pszRet;
}

/*
 * Prints the HTML formatted documentation for an enum value into a string buffer.
 */
static void macrodoc_printHelpForEnumValue(PARAMETER_ENUM_VALUE* pEnumValue, STRING_BUF* pBuf, BOOL bTopLevel) {
	if (bTopLevel) {
		stringbuf_appendString(pBuf, "</p><p><i>");
	} else {
		stringbuf_appendString(pBuf, "<li>");
	}
	stringbuf_appendString(pBuf, pEnumValue->pev_name);
	if (!bTopLevel) {
		stringbuf_appendString(pBuf, " = ");
		char szText[128];
		sprintf(szText, "0x%lx", pEnumValue->pev_val);
		stringbuf_appendString(pBuf, szText);
	}
	if (bTopLevel) {
		stringbuf_appendString(pBuf, "</i></p><p>");
	} else {
		stringbuf_appendString(pBuf, " - ");
	}
	stringbuf_appendString(pBuf, pEnumValue->pev_documentation ? pEnumValue->pev_documentation : "NO DESCRIPTION");
	stringbuf_appendString(pBuf, bTopLevel ? "</p>\n" : "</li>\n");
}

/*
 * Prints the HTML formatted documentation for a struct member into a string buffer.
 */
static void macrodoc_printHelpForStructMember(TYPE_PROPERTY_DESCRIPTOR* pProperty, STRING_BUF* pBuf) {
	stringbuf_appendString(pBuf, "<li>");
	stringbuf_appendString(pBuf, macrodoc_printTypeWithLink(types_nameFor(pProperty->tpd_type)));
	stringbuf_appendChar(pBuf, ' ');
	stringbuf_appendString(pBuf, pProperty->tpd_name);
	stringbuf_appendString(pBuf, " - ");
	stringbuf_appendString(pBuf, pProperty->tpd_documentation ? pProperty->tpd_documentation : "NO DESCRIPTION");
	stringbuf_appendString(pBuf, "</li>\n");
}

/*
 * Returns the pointer to an allocated string containing the HTML formatted
 * help for an enum value.
 */
const char* macrodoc_helpForEnumValue(const char* pszName, PARAMETER_ENUM_VALUE* pEnumValue) {
	STRING_BUF* pBuf = stringbuf_create(200);
	macrodoc_startHelpSection(pBuf, "Enum Value");
	macrodoc_printHelpForEnumValue(pEnumValue, pBuf, TRUE);
	char* pszRet = _strdup(stringbuf_getString(pBuf));
	stringbuf_destroy(pBuf);
	return pszRet;
}

/*
 * Returns the pointer to an allocated string containing the HTML formatted
 * help for a user defined macro.
 */
const char* macrodoc_helpForMacro(const char* pszName, MACRO* pMacro) {
	STRING_BUF* pBuf = stringbuf_create(200);

	macrodoc_startHelpSection(pBuf, "Synopsis");
	decompile_printMacroSignature(pMacro, pBuf, 0, macrodoc_printTypeWithLink);
	stringbuf_appendString(pBuf, "</p>");
	char* pszComment = MAC_COMMENT(pMacro);
	if (pszComment) {
		macrodoc_formatJavadocComment(pBuf, pszComment);
	}
	char* pszRet = _strdup(stringbuf_getString(pBuf));
	stringbuf_destroy(pBuf);
	return pszRet;
}

/*
 * Returns the pointer to an allocated string containing the documentation for
 * a PKSMacroC type with the given name 'pszType'.
 */
const char* macrodoc_helpForType(const char* pszType, void* pszUnused) {
	PKS_TYPE_DESCRIPTOR* pDescriptor = types_getTypeDescriptor(pszType);
	if (!pDescriptor) {
		return 0;
	}
	STRING_BUF* pBuf = stringbuf_create(200);
	stringbuf_appendString(pBuf, "<h5>");
	stringbuf_appendString(pBuf, pszType);
	stringbuf_appendString(pBuf, "</h5>");
	if (pDescriptor->ptd_documentation) {
		macrodoc_formatJavadocComment(pBuf, pDescriptor->ptd_documentation);
	}
	if (pDescriptor->ptd_numberOfProperties) {
		stringbuf_appendString(pBuf, "<ul>");
		for (int i = 0; i < pDescriptor->ptd_numberOfProperties; i++) {
			PARAMETER_ENUM_VALUE* pValue = &pDescriptor->ptd_elements.ptd_enumValues[i];
			if (pDescriptor->ptd_isEnumType) {
				macrodoc_printHelpForEnumValue(pValue, pBuf, FALSE);
			} else {
				macrodoc_printHelpForStructMember(((TYPE_PROPERTY_DESCRIPTOR*)pValue), pBuf);
			}
		}
		stringbuf_appendString(pBuf, "</ul>");
	}
	const char* pszRet = _strdup(stringbuf_getString(pBuf));
	stringbuf_destroy(pBuf);
	return pszRet;
}

/*
 * Returns a pointer to an allocated string containing the contents of the documentation
 * "pointer to" by a macrodoc hyperlink reference (macroref://typename or the like).
 */
const char* macrodoc_helpForHyperlink(const char* pszUrl) {
	int nSize = (int)strlen(MACROREF_PROTOCOL);
	if (pszUrl && strncmp(pszUrl, MACROREF_PROTOCOL, nSize) == 0) {
		char* ret;
		const char* pszName = pszUrl + nSize;
		char* pszDash = strchr(pszName, '#');
		if (pszDash) {
			pszDash++;
			PARAMETER_ENUM_VALUE* pValue = function_getParameterEnumValue(0, pszDash);
			if (pValue) {
				return macrodoc_helpForEnumValue(pszDash, pValue);
			}
		}
		SYMBOL sym = sym_find(sym_getGlobalContext(), pszName, &ret);
		if (sym.s_type == S_EDFUNC) {
			return macrodoc_helpForNativeFunction(pszName, (void*)VALUE(sym));
		}
		if (sym.s_type == S_TYPE_IDENTIFIER) {
			return (char*)macrodoc_helpForType(pszName, (void*)pszName);
		}
		for (int i = 0; i < macro_getNumberOfMacros(); i++) {
			MACRO* mp = macro_getByIndex(i);
			if (mp && strcmp(MAC_NAME(mp), pszName) == 0) {
				return macrodoc_helpForMacro(pszName, mp);
			}
		}
	}
	return 0;
}



