 /*
  * 
  *  Copyright (c) 2011
  *  name : mhogo mchungu 
  *  email: mhogomchungu@gmail.com
  *  This program is free software: you can redistribute it and/or modify
  *  it under the terms of the GNU General Public License as published by
  *  the Free Software Foundation, either version 2 of the License, or
  *  (at your option) any later version.
  * 
  *  This program is distributed in the hope that it will be useful,
  *  but WITHOUT ANY WARRANTY; without even the implied warranty of
  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  *  GNU General Public License for more details.
  * 
  *  You should have received a copy of the GNU General Public License
  *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
  */
 
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <libcryptsetup.h>

#include "../string/String.h" 
#include "../string/StringList.h" 
#include "../constants.h"
#include "../lib/libzuluCrypt.h"
#include "libzuluCrypt-exe.h"

#ifdef __STDC__
char * realpath( const char * path, char * resolved_path ) ;
#endif

/*
 * These functions are here and not in libzuluCrypt-exe.h because they use string_t and the type is not supposed to be
 * seen in public API
 */
/*
 * this function makes a unique mapper name based on user UID to make sure one user can not manage another user
 * opened volumes.
 * It is defined in create_mapper_name.c
 */
string_t create_mapper_name( const char * device,const char * mapping_name,uid_t uid,int ) ;

/*
 * this function is responsibe substituting bash special characters with an underscore.
 * The explanation for why it does that is in the source file.
 * The function is defined in replace_bash_special_chars.c * 
 */
void replace_bash_special_chars( string_t ) ;

/*
 * thiw function reads a passphrase from a key file after it makes sure a user who started the
 * tool had reading access to the file.
 * It is defined in security.c
 */
int get_pass_from_file( const char * path,uid_t uid,string_t * st ) ;

/*
 * this function is defined in module_system.c
 * It gets a passphrase from a .so file as a plugin.
 */
string_t GetKeyFromModule( const char * path,uid_t ) ;
