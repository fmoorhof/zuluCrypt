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

#include "includes.h"

#include <mntent.h>
#include <sys/mount.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
/*
 * below header file does not ship with the source code, it is created at configure time
 * */
#include "libmount_header.h"

static inline int _unmount_volume( const char * m_dir )
{
	int h ;
	int i ;
	
	/*
	 * try 5 times on one second intervals to umount the volume.
	 * Trying to unmount more than once seem to be necessary sometimes
	 *  when the opened volume is accessed over samba share. 
	 */
	for( i = 0 ; i < 5 ; i++ ){
		h = umount( m_dir ) ;
		if( h == 0 ){
			break ;
		}else{
			sleep( 1 ) ;
		}
	}
	
	return h ;
}

int zuluCryptUnmountVolume( const char * device,char ** m_point )
{
	char * m ;
	int h = 3 ;
	
	char * loop_path = NULL ;
	
	if( StringPrefixMatch( device,"/dev/loop",9 ) ){
		/*
		 * zuluCryptLoopDeviceAddress() is defined in ./create_loop_device.c
		 */
		loop_path = zuluCryptLoopDeviceAddress( device ) ;
		if( loop_path != NULL ){
			device = loop_path ;
		}
	}
	
	/*
	 * zuluCryptGetMountPointFromPath() is defined in ./process_mountinfo.c
	 */
	m = zuluCryptGetMountPointFromPath( device ) ;

	if( m != NULL ){
		h = _unmount_volume( m ) ;
		if( h == 0 ){
			if( m_point != NULL ){
				*m_point = m ;
			}else{
				StringFree( m ) ;
			}
		}else{
			StringFree( m ) ;
		}
	}
		
	if( h != 0 && h != 3 && h != 4 ){
		h = 2 ;
	}
	
	StringFree( loop_path ) ;
	
	return h ;
}
