/*
 * DecompileMacros.c
 *
 * decompile macros
 *
 * PROJECT: PKSEDIT
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * author : TOM
 * created: 13.03.1991
 */

#include <stdio.h>
#include <string.h>

#include "pksmacrocvm.h"
#include "pksmacro.h"
#include "funcdef.h"
#include "pksmod.h"
#include "symbols.h"
#include "test.h"
#include "funcdef.h"
#include "stringutil.h"
#include "errordialogs.h"

int  bytecode_makeAutoLabel(COM_GOTO *cp);
void bytecode_initializeAutoLabels(void);
void bytecode_startNextAutoLabel(char **name, COM_GOTO **cp);
void bytecode_closeAutoLabels(void);
char *bytecode_findAutoLabelForInstruction(COM_GOTO *cp);

/*
 * decompile_stringForTestOperator()
 */
static char *decompile_stringForTestOperator(unsigned char op)
{
	switch(op & (~0x10)) {
		case CT_AND:	return "&&";
		case CT_OR:	return "||";
		case CT_BRACKETS: return "()";
		case CT_NOT:   return "!";
		case CT_EQ:	return "==";
		case CT_NE:	return "!=";
		case CT_GT:	return ">";
		case CT_LT:	return "<";
		case CT_GE:	return ">=";
		case CT_LE:	return "<=";
		case CT_MATCH: return "~";
		case CT_NMATCH:return "!~";
		default: 		return "?";
	}
}

/*
 * decompile_getFunctionForIndex()
 */
static EDFUNC *decompile_getFunctionForIndex(int idx)
{
	if (idx >= 0 && idx < _functionTableSize) {
		return &_functionTable[idx];
	}
	return 0;
}


/*
 * decompile_quoteString()
 *
 * decompile_quoteString the following chars: <"\>
 */
static char *decompile_quoteString(unsigned char *name)
{	static char qbuf[512];
	unsigned char *d,*dend;
	
	for (d = qbuf, dend= &qbuf[sizeof qbuf-2]; d < dend && *name; name++) {
		if (*name == '\\' || *name == '"')
			*d++ = '\\';
		*d++ = *name;
	}
	*d = 0;
	return qbuf;
}

/*--------------------------------------------------------------------------
 * decompile_printParameterAsConstant()
 */
static int decompile_printParameterAsConstant(FILE *fp,long long val, PARAMETER_TYPE_DESCRIPTOR partyp)
{
	PARAMETER_ENUM_VALUE	*ep,*epend;
	int printed = 0;

	ep = partyp.pt_enumVal;
	epend = partyp.pt_enumVal + partyp.pt_enumCount;

	if (partyp.pt_type == PARAM_TYPE_BITSET) {
		while(ep < epend) {
			if (val & ep->pev_val) {
				if (printed)
					fputc('|',fp);
				fputs(ep->pev_name,fp);
				printed++;
				val &= (~ep->pev_val);
				if (!val)
					return 1;
			}
			ep++;
		}
	} else {
		/* OT_ENUM */
		while(ep < epend) {
			if (val == ep->pev_val) {
				fputs(ep->pev_name,fp);
				return 1;
			}
			ep++;
		}
	}
	if (val || !printed) {
		if (printed)
			fputc('|',fp);
		fprintf(fp,"%lld",val);
	}
	return 1;
}

/*
 * decompile_printParameter()
 */
static int decompile_printParameter(FILE *fp, unsigned char *sp, PARAMETER_TYPE_DESCRIPTOR partyp)
{
	unsigned char typ;
	long long val;

	if (((intptr_t)sp) & 1)
		goto err;

	typ = ((COM_CHAR1 *)sp)->typ;
	switch(typ) {
		case C_FORMSTART:
			val = ((COM_FORM *)sp)->options;
			break;
		case C_1FUNC:
			val = ((COM_1FUNC *)sp)->p;
			break;
		case C_CHARACTER_LITERAL:
			val = ((COM_CHAR1 *)sp)->val;
			break;
		case C_FLOAT_LITERAL:
			fprintf(fp, "%lf", ((COM_FLOAT1*)sp)->val);
			return 1;
		case C_BOOLEAN_LITERAL:
			fprintf(fp, "%s", ((COM_FLOAT1*)sp)->val ? "true" : "false");
			return 1;
		case C_INTEGER_LITERAL:
			val = ((COM_INT1 *)sp)->val;
			break;
		case C_LONG_LITERAL:
			val = (long long)((COM_LONG1 *)sp)->val;
			break;
		case C_STRING_LITERAL:
			if (partyp.pt_type != PARAM_TYPE_STRING)
				goto err;
			fprintf(fp,"\"%s\"",decompile_quoteString(((COM_STRING1*)sp)->s));
			return 1;
		case C_FLOATVAR:
		case C_LONGVAR:
			if (partyp.pt_type == PARAM_TYPE_STRING)
				goto err;
			// drop through
		case C_STRINGVAR:
			fputs(((COM_VAR*)sp)->name,fp);
			return 1;
		default:
			err:
			error_displayAlertDialog("Format error in parameters of function to decompile");
			return -1;
	}

	if (partyp.pt_type == PARAM_TYPE_STRING)
		goto err;
	
	if (partyp.pt_type == PARAM_TYPE_ENUM && partyp.pt_enumVal)
	    return decompile_printParameterAsConstant(fp,val,partyp);

	if (typ == C_CHARACTER_LITERAL && val >= 32 && val <= 127) {
		if ((char)val == '\\') {
			fprintf(fp, "'\\\\'");
		} else {
			fprintf(fp, "'%c'", (char)val);
		}
	} else {
		fprintf(fp, "%lld", val);
	}
	return 1;
}

/*
 * decompile_printFunctionName
 */
static void decompile_printFunctionName(FILE *fp, int idx)
{
	EDFUNC	*ep = decompile_getFunctionForIndex(idx);

	if (!ep) {
		error_displayAlertDialog("format error: bad function");
		return;
	}
	fprintf(fp,"\t%s",macro_loadStringResource(idx));
}

/*
 * decompile_makeAutoLabels()
 */
static void decompile_makeAutoLabels(const char *start, const char *end)
{
	while(start < end && *start != C_STOP) {
		if (*start == C_GOTO)
			bytecode_makeAutoLabel((COM_GOTO*)(start+((COM_GOTO *)start)->offset));
		start += macro_getParameterSize(*start,start+1);
	}
}

/*
 * decompile_function()
 */
static unsigned char *decompile_function(FILE *fp, unsigned char *sp, 
								    unsigned char *spend)
{
	COM_1FUNC *		cp = (COM_1FUNC *)sp;
	EDFUNC	*		ep;
	int 			npars;
	PARAMETER_TYPE_DESCRIPTOR partyp;

	if (cp->typ == C_MACRO) {
		fprintf(fp,"\t%s()",((COM_MAC*)sp)->name);
		return sp+macro_getParameterSize(*sp,sp+1);
	}

	decompile_printFunctionName(fp,cp->funcnum);
	ep = decompile_getFunctionForIndex(cp->funcnum);
	fputc('(',fp);

	npars = 0;
	if (cp->typ == C_1FUNC) {
		partyp = function_getParameterTypeDescriptor(ep, 1);
		/* c0_func glitch */
		if (partyp.pt_type != PARAM_TYPE_VOID &&
		    (partyp.pt_enumVal == NULL)) {
			if (decompile_printParameter(fp,sp,partyp) < 0)
				return 0;
			npars++;
		}
	}
	sp += macro_getParameterSize(*sp,sp+1);

	while(sp < spend && C_IS1PAR(*sp)) {
		partyp = function_getParameterTypeDescriptor(ep, npars + 1);
		if (partyp.pt_type != PARAM_TYPE_VOID) {
			if (npars)
				fputc(',',fp);
			if (decompile_printParameter(fp,sp,partyp) < 0)
				return 0;
			npars++;
		}
		sp += macro_getParameterSize(*sp,sp+1);
	}

	fputc(')',fp);
	return sp;
}

/*
 * decompile_printTestExpression()
 */
static void decompile_printTestExpression(FILE *fp, unsigned char *sp,unsigned char *spend)
{	unsigned char t;

	t = ((COM_1FUNC*)sp)->typ;
	if (C_ISCMD(t))
		decompile_function(fp,sp,spend);
	else {
		decompile_printParameter(fp, sp, (PARAMETER_TYPE_DESCRIPTOR) { .pt_type = macro_isParameterStringType(*sp) ? PARAM_TYPE_STRING : PARAM_TYPE_INT });
	}
}

/*
 * decompile_printExpression()
 */
static unsigned char *decompile_printExpression(FILE *fp, COM_TEST *cp, int not)
{	unsigned char *p2,*pend;
	COM_TEST *p1;
	int top;

	p1 = cp+1;
	p2 = (unsigned char *)cp + cp->p2offset;
	pend = (unsigned char *)cp + cp->size;
	switch(cp->testop & (~CT_STRING)) {
		case CT_NOT:
			if (!not)
				fprintf(fp,"%s",decompile_stringForTestOperator(cp->testop));
			decompile_printExpression(fp,p1,0);
			break;			
		case CT_OR:
		case CT_AND:
			decompile_printExpression(fp,p1,0);
			fprintf(fp,"%s\n\t\t",decompile_stringForTestOperator(cp->testop));
			if (p2 > (unsigned char *)p1)
				decompile_printExpression(fp,(COM_TEST*)p2,0);
			else
				error_displayAlertDialog("expr: bad AND/OR|(p2 == 0x%lx)(p1 == 0x%lx)",
					p2,p1);
			break;
		case CT_BRACKETS:
			fprintf(fp,"(");
			decompile_printExpression(fp,p1,not);
			fprintf(fp,")");
			break;
		case CT_GT:	top = (not) ? CT_LE : CT_GT; goto prdo;
		case CT_LT:	top = (not) ? CT_GE : CT_LT; goto prdo;
		case CT_EQ:	top = (not) ? CT_NE : CT_EQ; goto prdo;
		case CT_NE:	top = (not) ? CT_EQ : CT_NE; goto prdo;
		case CT_MATCH:	top = (not) ? CT_NMATCH : CT_MATCH; goto prdo;
		case CT_NMATCH:top = (not) ? CT_MATCH : CT_NMATCH;
prdo:		decompile_printTestExpression(fp,(unsigned char *)p1,p2);
			fprintf(fp," %s ",decompile_stringForTestOperator(top));
			decompile_printTestExpression(fp,p2,pend);
			break;
		default:
			fprintf(fp,"?expr 0x%x?",cp->testop);
	}
	return pend;
}

/**
 * Return a type name for a symbol type.
 */
static char* decompile_typenameFor(PKS_VALUE_TYPE nSymbolType) {
	switch (nSymbolType) {
	case VT_FLOAT: return "float";
	case VT_INTEGER: return "int";
	case VT_BOOLEAN: return "boolean";
	default: return "string";
	}
}

/*
 * decompile_printBinaryExpression()
 */
static unsigned char *decompile_printBinaryExpression(FILE *fp, COM_BINOP *cp)
{
	unsigned char *	p1;
	unsigned char *	p2;
	unsigned char *	pend;

	p1 = (unsigned char *)(cp)+cp->op1offset;
	pend = (unsigned char *)(cp)+cp->size;
	p2 = (unsigned char *)(cp)+cp->op2offset;

	fprintf(fp,"\t%s = ",cp->result);

	switch(cp->op) {

	case BIN_BRACKETS: 
		fprintf(fp,"("); 
		pend = decompile_printBinaryExpression(fp,(COM_BINOP*)p1);
		fprintf(fp,")"); 
		return pend;
	case BIN_XOR: 
	case BIN_ADD: 
	case BIN_SUB: 
	case BIN_MUL: 
	case BIN_DIV: 
	case BIN_MOD: 
		decompile_printTestExpression(fp,p1,p2);
		fprintf(fp,"%c",cp->op);
		decompile_printTestExpression(fp,p2,pend);
		break;
	case BIN_CAST:
		fprintf(fp,"(%s) %s", decompile_typenameFor(*p1), cp->result);
		break;
	}

	return pend;
}


/*
 * Return a string representation of a BINOP instruction.
 */
static char* decompile_binaryOperationAsString(unsigned char op) {
	static char szTemp[2];

	if (op == BIN_CAST) {
		return "cast";
	}
	szTemp[0] = op;
	return szTemp;
}

/*
 * Return a string representation of a test instruction.
 */
static char* decompile_testOperationAsString(unsigned char op) {

	switch (op & (~CT_STRING)) {
	case CT_AND: return "and";
	case CT_OR: return "or";
	case CT_BRACKETS: return "()";
	case CT_EQ: return "==";
	case CT_NE: return "!=";
	case CT_GE: return ">=";
	case CT_GT: return ">";
	case CT_LE: return "<=";
	case CT_LT: return "<";
	case CT_NMATCH: return "not rematch";
	case CT_MATCH: return "rematch";
	case CT_NOT: return "not";
	}
	return "??";
}


/*
 * decompile_macroInstructionsToFile
 */
static void (*_decompileFunction)(FILE* fp, MACRO* mp);
static void decompile_macroInstructionsToFile(FILE* fp, MACRO* mp)
{
	unsigned char* sp, * spend, * data;
	MACROC_INSTRUCTION_OP_CODE t;

	fprintf(fp, "%s:", decompile_quoteString(MAC_NAME(mp)));
	data = MAC_DATA(mp);

	for (sp = data, spend = sp + mp->size; sp < spend; ) {
		t = ((COM_1FUNC*)sp)->typ;
		int nOffs = (int)(sp - data);
		fprintf(fp, "\n0x%04x:   ", nOffs);
		switch (t) {
		case C_STOP: fprintf(fp, "stop"); break;
		case C_TEST: fprintf(fp, "testAndGoto %s, 0x%x", decompile_testOperationAsString(((COM_TEST*)sp)->testop), nOffs + ((COM_TEST*)sp)->size); break;
		case C_GOTO: fprintf(fp, "goto 0x%x", nOffs + ((COM_GOTO*)sp)->offset); break;
		case C_BINOP: fprintf(fp, "eval %s", decompile_binaryOperationAsString(((COM_BINOP*)sp)->op)); break;
		case C_ASSIGN: fprintf(fp, "assignVar %s", ((COM_ASSIGN*)sp)->name); break;
		case C_MACRO:fprintf(fp, "executeMacro %s", ((COM_MAC*)sp)->name); break;
		case C_0FUNC: fprintf(fp, "param0Fun %d", ((COM_0FUNC*)sp)->funcnum); break;
		case C_1FUNC: fprintf(fp, "param1Fun %d %d", ((COM_1FUNC*)sp)->funcnum, ((COM_1FUNC*)sp)->p); break;
		case C_BOOLEANVAR: fprintf(fp, "bvar "); break;
		case C_STRINGVAR: fprintf(fp, "svar "); break;
		case C_FLOATVAR: fprintf(fp, "fvar "); break;
		case C_LONGVAR: fprintf(fp, "lvar "); break;
		case C_LONG_LITERAL: fprintf(fp, "longLiteral %lld", (long long)((COM_LONG1*)sp)->val); break;
		case C_INTEGER_LITERAL: fprintf(fp, "intLiteral %d", ((COM_INT1*)sp)->val); break;
		case C_FLOAT_LITERAL: fprintf(fp, "floatLiteral %f", ((COM_FLOAT1*)sp)->val); break;
		case C_BOOLEAN_LITERAL: fprintf(fp, "booleanLiteral %d", ((COM_CHAR1*)sp)->val); break;
		case C_CHARACTER_LITERAL: fprintf(fp, "charLiteral %d", ((COM_CHAR1*)sp)->val); break;
		case C_STRING_LITERAL: fprintf(fp, "stringLiteral %s", ((COM_STRING1*)sp)->s); break;
		case C_DEFINE_VARIABLE: fprintf(fp, "defineVar %s", ((COM_CREATESYM*)sp)->name); break;
		case C_DEFINE_PARAMETER: fprintf(fp, "defineParam %s", ((COM_CREATESYM*)sp)->name); break;
		default: fprintf(fp, "opcode 0x%x", t); break;
		}
		if (t == C_STRINGVAR || t == C_BOOLEANVAR || t == C_FLOATVAR || t == C_LONGVAR) {
			decompile_printParameter(fp, sp, (PARAMETER_TYPE_DESCRIPTOR) { .pt_type = macro_isParameterStringType(t) ? PARAM_TYPE_STRING : PARAM_TYPE_INT });
		}
		sp += macro_getParameterSize(*sp, sp+1);
	}
}

/*
 * decompile_macroToFile
 */
#define COM1_INCR(cp,type,offset) (unsigned char *)(cp+((type *)cp)->offset)
static void decompile_macroToFile(FILE *fp, MACRO *mp)
{	unsigned char 	*sp,*sp2,*spend,*gop,*data,*comment;
	unsigned char 	t;
	char			*lname;

	fprintf(fp,"macro %s()",decompile_quoteString(MAC_NAME(mp)));
	comment = MAC_COMMENT(mp);
	if (*comment)
		fprintf(fp," \"%s\"",decompile_quoteString(comment));
	fprintf(fp," {\n");
	data = MAC_DATA(mp);

	decompile_makeAutoLabels(data,&data[mp->size]);
	bytecode_initializeAutoLabels();
	bytecode_startNextAutoLabel(&lname,(COM_GOTO**)&gop);

	for (sp = data, spend = sp+mp->size; sp < spend; ) {
		if (gop <= sp && lname) {
			if (gop < sp) {
				error_displayAlertDialog("format error: bad goto");
			}
			fprintf(fp,"%s:\n",lname);
			bytecode_startNextAutoLabel(&lname, (COM_GOTO**)&gop);
		}

		t = ((COM_1FUNC*)sp)->typ;
		if (C_ISCMD(t)) {
			if ((sp = decompile_function(fp,sp,spend)) == 0)
				goto out;
			fprintf(fp,";\n");
			continue;
		}
		if (t == C_GOTO) {
			COM_GOTO *cp = (COM_GOTO *)sp;

			fprintf(fp,"\t%s %s;\n",
				(cp->bratyp == BRA_ALWAYS) ? "goto" : "braeq",
			bytecode_findAutoLabelForInstruction((COM_GOTO*)(sp+cp->offset)));
		} else if (t == C_STOP) {
			if (sp + sizeof(COM_STOP) < spend)
				fprintf(fp,"\treturn %d;\n",((COM_STOP*)sp)->rc);
		} else if (t == C_TEST) {
			COM_GOTO *cp =  (COM_GOTO *)(sp + ((COM_TEST*)sp)->size);

			/*
			 * generated if,while code
			 */
			if (cp->typ == C_GOTO &&
			    cp->bratyp == BRA_EQ) {
				fprintf(fp,"\tif ");
				sp = decompile_printExpression(fp,(COM_TEST*)sp,1);
				fprintf(fp,"\n\t\tgoto %s;\n",
						bytecode_findAutoLabelForInstruction((COM_GOTO*)(sp+cp->offset)));
				sp = (unsigned char *)(cp+1);
			} else {
				fprintf(fp,"\tif ");
				sp = decompile_printExpression(fp,(COM_TEST*)sp,0);
				fprintf(fp,"\n");
			}
			continue;
		} else if (t == C_BINOP) {
			decompile_printBinaryExpression(fp, (COM_BINOP*)sp);
			fprintf(fp,";\n");
			sp = COM1_INCR(sp,COM_BINOP,size);
			continue;
		} else if (t == C_ASSIGN) {
			fprintf(fp,"\t%s = ",((COM_ASSIGN*)sp)->name);
			sp2 = COM1_INCR(sp,COM_ASSIGN,size);
			decompile_printTestExpression(fp,COM1_INCR(sp,COM_ASSIGN,opoffset),sp2);
			sp = sp2;
			fprintf(fp,";\n");
			continue;
		} else if (t == C_DEFINE_PARAMETER) {
			sp = COM1_INCR(sp, COM_CREATESYM, size);
		} else if (t == C_DEFINE_VARIABLE) {
			char* pszType = decompile_typenameFor(((COM_CREATESYM*)sp)->symtype);
			fprintf(fp, "\t%s %s;\n", pszType, ((COM_CREATESYM*)sp)->name);
			sp = COM1_INCR(sp, COM_CREATESYM, size);
		} else if (t == C_FURET) {
		
		} else {
			error_displayAlertDialog("format error in %s type=%x",MAC_NAME(mp),t);
		}
		sp += macro_getParameterSize(*sp,sp+1);
	}

out:
	fprintf(fp,"}\n\n");
	bytecode_closeAutoLabels();
}

/*
 * decompile_printMacrosCallback
 */
static char *_macroname;
static long decompile_printMacrosCallback(FILE *fp)
{	MACRO	**mtable,*mp;
	int	    	i;

	for (mtable = _macroTable, i = 0; i < _macroTableSize; i++) {
		if ((mp = mtable[i]) != 0) {
			if (!_macroname ||
			    strcmp(_macroname,MAC_NAME(mp)) == 0)
				_decompileFunction(fp,mp);
		}
	}
	return 1;
}

/*
 * decompile_saveMacrosAndDisplay
 * Decompiles one or all macros and prints the decompilation result to a given file.
 * Decompilation may be performed by printing the code as source code or by printing the low level instructions
 */
int decompile_saveMacrosAndDisplay(char *macroname, DECOMPILATION_MODE nMode) {
	char szBuf[128];
	char* pszExtension = nMode == DM_CODE ? ".pkc" : ".pkobj";

	_macroname = macroname;
	if (macroname) {
		strmaxcpy(szBuf, macroname, sizeof szBuf-5);
	} else {
		strcpy(szBuf, "macros");
	}
	strcat(szBuf, pszExtension);
	_decompileFunction = nMode == DM_CODE ? decompile_macroToFile : decompile_macroInstructionsToFile;
	return macro_createFileAndDisplay(szBuf, decompile_printMacrosCallback);
}


