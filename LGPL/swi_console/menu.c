/*  $Id: menu.c,v 1.1 2008-03-27 00:41:33 vsc Exp $

    Part of SWI-Prolog

    Author:        Jan Wielemaker
    E-mail:        jan@swi.psy.uva.nl
    WWW:           http://www.swi-prolog.org
    Copyright (C): 1985-2002, University of Amsterdam

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <windows.h>
#include <tchar.h>
#define _MAKE_DLL
#include "console.h"
#include "console_i.h"
#include "menu.h"

#ifndef EOS
#define EOS 0
#endif

#define streq(s,q) (_tcscmp((s), (q)) == 0)

static TCHAR **menuids;
static int nmenus;
static int nmenualloc;

static struct rl_item
{ UINT	      id;
  const TCHAR *name;
} rl_items[] =
{ { IDM_EXIT, _T("&Exit") },
  { IDM_CUT,  _T("&Cut") },
  { IDM_COPY, _T("&Copy") },
  { IDM_PASTE, _T("&Paste") },
  { IDM_BREAK, _T("&Interrupt") },
  { IDM_FONT,  _T("&Font ...") },
  { 0,         NULL }
};



static UINT
lookupMenuLabel(const TCHAR *label)
{ int i;
  size_t llen;
  struct rl_item *builtin;

  for(builtin = rl_items; builtin->id; builtin++)
  { if ( streq(builtin->name, label) )
      return builtin->id;
  }

  for(i=0; i<nmenus; i++)
  { if ( streq(menuids[i], label) )
      return i + IDM_USER;
  }

  if ( nmenus + 1 > nmenualloc )
  { if ( nmenualloc )
    { nmenualloc *= 2;
      menuids = rlc_realloc(menuids, nmenualloc*sizeof(TCHAR *));
    } else
    { nmenualloc = 32;
      menuids = rlc_malloc(nmenualloc*sizeof(TCHAR *));
    }
  }

  llen = _tcslen(label);
  menuids[nmenus] = rlc_malloc((llen+1)*sizeof(TCHAR));
  _tcsncpy(menuids[nmenus], label, llen+1);
  
  return nmenus++ + IDM_USER;
}

const TCHAR *
lookupMenuId(UINT id)
{ struct rl_item *builtin;

  if ( id >= IDM_USER && (int)id - IDM_USER < nmenus )
    return menuids[id-IDM_USER];

  for(builtin = rl_items; builtin->id; builtin++)
  { if ( builtin->id == id )
      return builtin->name;
  }

  return NULL;
}

int
insertMenu(HMENU in, const TCHAR *label, const TCHAR *before)
{ if ( !before )
  { if ( !label )
      AppendMenu(in, MF_SEPARATOR, 0, NULL);
    else
    { UINT id = lookupMenuLabel(label);
      
      AppendMenu(in, MF_STRING, id, label);
    }
  } else
  { UINT bid = lookupMenuLabel(before);
    MENUITEMINFO info;

    memset(&info, 0, sizeof(info));
    info.cbSize = sizeof(info);
    info.fMask = MIIM_TYPE;
    if ( label )
    { info.fType = MFT_STRING;
      info.fMask |= MIIM_ID;
      info.wID = lookupMenuLabel(label);
      info.dwTypeData = (TCHAR *)label;
      info.cch = (int)_tcslen(label);
    } else
    { info.fType = MFT_SEPARATOR;
    }

    InsertMenuItem(in, bid, FALSE, &info);
  }

  return TRUE;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Find popup with given name.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

static HMENU
findPopup(RlcData b, const TCHAR *name, int *pos)
{ HMENU mb = GetMenu(rlc_hwnd(b));

  if ( mb )
  { int i;
    MENUITEMINFO info;

    memset(&info, 0, sizeof(info));
    info.cbSize = sizeof(info);
    info.fMask  = MIIM_TYPE;

    for(i=0; ; i++)
    { MENUITEMINFO info;
      TCHAR nbuf[MAXLABELLEN];

      memset(&info, 0, sizeof(info));
      info.cbSize = sizeof(info);
      info.fMask  = MIIM_TYPE|MIIM_SUBMENU;
      info.dwTypeData = nbuf;
      info.cch = sizeof(nbuf);

      if ( !GetMenuItemInfo(mb, i, TRUE, &info) )
	return NULL;

      if ( info.fType == MF_STRING )
      { if ( streq(name, nbuf) )
	{ if ( pos )
	    *pos = i;

	  return info.hSubMenu;
	}
      }
    }
  }

  return 0;
}


static void
append_builtin(HMENU menu, UINT id)
{ AppendMenu(menu, MF_STRING, id, lookupMenuId(id));
}


void
rlc_add_menu_bar(HWND cwin)
{ HMENU menu     = CreateMenu();
  HMENU file	 = CreatePopupMenu();
  HMENU edit     = CreatePopupMenu();
  HMENU settings = CreatePopupMenu();
  HMENU run      = CreatePopupMenu();

  append_builtin(file, IDM_EXIT);

/*append_builtin(edit, IDM_CUT);*/
  append_builtin(edit, IDM_COPY);
  append_builtin(edit, IDM_PASTE);
  
  append_builtin(settings, IDM_FONT);

  append_builtin(run,  IDM_BREAK);

  AppendMenu(menu, MF_POPUP, (UINT)file,     _T("&File"));
  AppendMenu(menu, MF_POPUP, (UINT)edit,     _T("&Edit"));
  AppendMenu(menu, MF_POPUP, (UINT)settings, _T("&Settings"));
  AppendMenu(menu, MF_POPUP, (UINT)run,      _T("&Run"));

  SetMenu(cwin, menu);
}

		 /*******************************
		 *	    EXTERNAL		*
		 *******************************/

#define MEN_MAGIC 0x6c4a58e0

void
rlc_menu_action(rlc_console c, menu_data *data)
{ RlcData b = rlc_get_data(c);

  if ( !data || !data->magic == MEN_MAGIC )
    return;
  
  if ( data->menu )			/* rlc_insert_menu_item() */
  { HMENU popup;

    if ( (popup = findPopup(b, data->menu, NULL)) )
      data->rc = insertMenu(popup, data->label, data->before);
    else
      data->rc = FALSE;
  } else				/* insert_menu() */
  { HMENU mb;
    HWND hwnd = rlc_hwnd(c);

    if ( !(mb = GetMenu(hwnd)) )
    { data->rc = FALSE;
      return;
    }

    if ( !findPopup(b, data->label, NULL) )	/* already there */
    { MENUITEMINFO info;
      int bid = -1;

      if ( data->before )
	findPopup(b, data->before, &bid);

      memset(&info, 0, sizeof(info));
      info.cbSize = sizeof(info);
      info.fMask = MIIM_TYPE|MIIM_SUBMENU;
      info.fType = MFT_STRING;
      info.hSubMenu = CreatePopupMenu();
      info.dwTypeData = (TCHAR *)data->label;
      info.cch = (int)_tcslen(data->label);
      
      InsertMenuItem(mb, bid, TRUE, &info);
					/* force redraw; not automatic! */
      DrawMenuBar(hwnd);
    }

    data->rc = TRUE;
  }
}


int
rlc_insert_menu(rlc_console c, const TCHAR *label, const TCHAR *before)
{ HWND hwnd = rlc_hwnd(c);
  menu_data data;

  data.magic  = MEN_MAGIC;
  data.menu   = NULL;
  data.label  = label;
  data.before = before;

  SendMessage(hwnd, WM_RLC_MENU, 0, (LPARAM)&data);

  return data.rc;
}


int
rlc_insert_menu_item(rlc_console c,
		     const TCHAR *menu, const TCHAR *label, const TCHAR *before)
{ HWND hwnd = rlc_hwnd(c);
  menu_data data;

  data.magic  = MEN_MAGIC;
  data.menu   = menu;
  data.label  = label;
  data.before = before;

  SendMessage(hwnd, WM_RLC_MENU, 0, (LPARAM)&data);

  return data.rc;
}
