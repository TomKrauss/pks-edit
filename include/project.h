/*
 * project.h
 *
 * comment: manage project groups in PKS-EDIT
 *
 * PROJECT: PKSEDIT
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * author: Tom
 * created:  08.01.1993
 */

# if !defined( __PROJECT_H )
#define __PROJECT_H

typedef struct tagPROJECTITEM {
	char	*		pi_name;			/* filename */
	void *		pi_doctype;		/* document type of that file */
} PROJECTITEM;

typedef struct tagPROJECT {
	struct tagPROJECT *pr_next;
	char 		pr_name[32];		/* name of project */
	char *		pr_description;	/* visual description */
	char *		pr_rootdir;		/* root directory of project */
	void *		pr_doctype;		/* standard document type for project */
	int			pr_nitems;		/* # of project items */
	int			pr_nhsize;		/* size of project item hash table */
	PROJECTITEM *	pr_itemtable;		/* hashed table of project items */
} PROJECT;


PROJECTITEM *		proj_finditem(char *itemname);
void				proj_setcurrent(char *name);


#endif /* __PROJECT_H*/

