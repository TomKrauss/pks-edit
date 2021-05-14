
#ifndef EDLBOX_H

/*
 * callback message types
 */
# define	LBN_DBCLK			1	/* item was double clicked */
# define	LBN_BUTTON			2	/* any non listbox-button in form */
# define	LBN_SELCHANGE		3	/* selection item has changed */
# define	LBN_DONE			4	/* selection dialog will terminate */

/* 
 * Parameters to callback:
 * DBCLK: 	LISTBOX OBTREE button
 * BUTTON:	LISTBOX OBTREE button
 * SELCHANGE:	LISTBOX OBTREE redrawflag
 *
 */

typedef struct listbox {
	int		treenum;
	int		ob1,obn,
			obslidepa,obslideson,
			obarrup,obarrdown,
			obok, obcancel;
	int 		(*getitem)(OBJECT *op, void *par, int item);
	int		(*callback)(int msgtype, struct listbox *lp, OBJECT *tree, int v);

	int		draw_ypos;			/* place of formular or -1 */
	int		currsel,currtop;
	int		max;
	void		*data;
} LISTBOX;

# define	EDLBOX_H
#endif

