/*
 * ManageProjectGroups.c
 *
 * comment: manage project groups in PKS-EDIT
 *
 * PROJECT: PKSEDIT
 *
 * (c) Pahlen & Krauﬂ					Author : TOM
 * All Rights Reserved.					created:  08.01.1993
 *
 */

#include <string.h>

#include "lineoperations.h"
#include "project.h"
#include "pathname.h"

extern void *	prof_llinsert(void *Head, int size, 
				char *group, char *item, char **idata);
extern void *	GetPrivateDocumentType(char *name);
extern int 	prof_enum(char *grp, int (*lpfnEnum)(char *, long), long lParam);

static	PROJECT	*projectList;

/*---------------------------------*/
/* tags_mk()					*/
/*---------------------------------*/
static char *szProj = "projects";
static int proj_mk(char *project, long dummy)
{
	char	*	s;
	char *	szDocType;
	PROJECT *	pp;

	if ((pp = prof_llinsert(&projectList, sizeof *pp, szProj, project, &s)) == 0) {
		return 0;
	}

	pp->pr_description = strtok(s, ",");
	pp->pr_rootdir = strtok((char *)0, ",");
	if ((szDocType = strtok((char *)0, ",")) != 0) {
		pp->pr_doctype = GetPrivateDocumentType(szDocType);
	}

	return 1;
}

/*--------------------------------------------------------------------------
 * xref_initFileFormats()
 * init all compiler types
 */
int proj_init(void)
{
	return prof_enum(szProj, proj_mk, 0L);
}

/*--------------------------------------------------------------------------
 * pr_matchrootdir()
 */
static int pr_matchrootdir(char *rootdir, char *name, int *pnOffset)
{
	char *startname;

	if (!rootdir) {
		return 0;
	}

	startname = name;

	while(*rootdir) {
		if (*rootdir++ != *name++) {
			return 0;
		}
	}

	if (*name == DIRSEPARATOR) {
		name++;
	}
	*pnOffset = (int)(name - startname);
	return 1;
}

/*--------------------------------------------------------------------------
 * proj_finditem()
 */
PROJECTITEM *proj_finditem(char *itemname)
{
	PROJECT *		pp;
	int			nNameOffset;

	for (pp = projectList; pp; pp = pp->pr_next) {
		if (pr_matchrootdir(pp->pr_rootdir, itemname, &nNameOffset)) {
#if 0
			for (nItem = 0; nItem < pp->pr_nhsize; nItem++) {
				pip = &pp->pr_itemtable[nItem];
				if (pip->pi_name && 
					strcmp(pip->pi_name, itemname) == 0) {
					return pip;
				}
			}
#else
	{
		static PROJECTITEM	pitem;
		
		pitem.pi_doctype = pp->pr_doctype;
		return &pitem;
	}
#endif
		}
	}
	return (PROJECTITEM *)0;
}

