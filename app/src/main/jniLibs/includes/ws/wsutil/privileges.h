/* privileges.h
 * Declarations of routines for handling privileges.
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 2006 Gerald Combs
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __PRIVILEGES_H__
#define __PRIVILEGES_H__

#include <ws/ws_symbol_export.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Called when the program starts, to enable security features and save
 * whatever credential information we'll need later.
 */
WS_DLL_PUBLIC void init_process_policies(void);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PRIVILEGES_H__ */
