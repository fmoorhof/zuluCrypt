if(${CMAKE_VERSION} VERSION_GREATER "3.26")
    cmake_minimum_required(VERSION 3.26.0)
else()
    cmake_minimum_required(VERSION 3.0.2)
endif()

set_property(GLOBAL PROPERTY ALLOW_DUPLICATE_CUSTOM_TARGETS ON)

project(zuluCrypt)

INCLUDE(GNUInstallDirs)

set( PGR_VERSION "7.1.0" )
set( LIB_VERSION "1.2.0" )
set( LIB_PLUGIN_VERSION "1.0.0" )
set( COPYRIGHT_YEARS "2011-2024" )

#add_definitions( -D_DEFAULT_SOURCE -fstack-protector-all -D_FORTIFY_SOURCE=2 --param ssp-buffer-size=4 )
add_definitions( -D_DEFAULT_SOURCE -fstack-protector-all --param ssp-buffer-size=4 )

include_directories( ${PROJECT_BINARY_DIR} )

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_definitions(-Qunused-arguments)
endif()

if( LIB_SUFFIX )
	set( CMAKE_INSTALL_LIBDIR "${LIB_SUFFIX}" )
	set( CMAKE_INSTALL_FULL_LIBDIR "${CMAKE_INSTALL_PREFIX}/${LIB_SUFFIX}" )
endif()

# These variables are used but they dont always appear to be set and we set them if we found them to be not set
if( NOT CMAKE_INSTALL_DOCDIR )
	set( CMAKE_INSTALL_DOCDIR "share/doc/zuluCrypt" )
endif()

if( NOT CMAKE_INSTALL_MANDIR )
	set( CMAKE_INSTALL_MANDIR "share/man" )
endif()

if( NOT CMAKE_INSTALL_DATADIR )
	set( CMAKE_INSTALL_DATADIR "share" )
endif()

# uninstall target
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
	IMMEDIATE @ONLY )

add_custom_target( uninstall
COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake )

INCLUDE(CMakeDependentOption)
INCLUDE(FindPkgConfig)

# options
option(NOGUI "Don't build the GUI" OFF)
cmake_dependent_option(WITH_PWQUALITY "Build with pwquality support" ON
	"NOT NOGUI" OFF)
option(NOGNOME "Build without gnome support" OFF)
cmake_dependent_option(NOKDE "Build without kde support" OFF
	"NOT NOGUI" ON)
option(UDEVSUPPORT "Build without udev support" ON)

if( NOT NOGNOME )
	pkg_check_modules( LIBSECRET libsecret-1 )
else( NOT NOGNOME )
	set( NOSECRETSUPPORT "true" )
endif( NOT NOGNOME )

if( NOKDE )
	set( NOKDESUPPORT "true" )
endif( NOKDE )

pkg_check_modules( CRYPTSETUP libcryptsetup )
pkg_check_modules( BLKID blkid )
pkg_check_modules( DEVMAPPER devmapper )
pkg_check_modules( UUID uuid )
pkg_check_modules( UUID-OSSP ossp-uuid )

pkg_check_modules( MCHUNGU_TASK mhogomchungu_task )

if ( WITH_PWQUALITY )
	pkg_check_modules( PWQUALITY pwquality )
endif ( WITH_PWQUALITY )

file( WRITE ${PROJECT_BINARY_DIR}/version_1.h "
#ifndef THIS_VERSION
#define THIS_VERSION \"${PGR_VERSION}\"
#endif
\n" )

file( WRITE ${PROJECT_BINARY_DIR}/version.h "
#ifndef ZULUCRYPT_VERSION
#define ZULUCRYPT_VERSION
#define VERSION_STRING \"Version  : ${PGR_VERSION}\\nCopyright: ${COPYRIGHT_YEARS} Francis Banyikwa, mhogomchungu@gmail.com\\nLicense  : GPLv2+\"
#endif
\n" )

if( NOT DEVMAPPER_FOUND )
	message( FATAL_ERROR "ERROR: could not find devmapper package" )
else( )
	find_library( devmapper_lib libdevmapper.so )
	if( NOT devmapper_lib )
		message( FATAL_ERROR "ERROR: could not find libdevmapper.so" )
	else()
		message( STATUS "Found version \"${DEVMAPPER_VERSION}\" of libdevmapper library: ${devmapper_lib}" )
	endif()
endif()

if( NOT UUID_FOUND )
	if( NOT UUID-OSSP_FOUND )
		message( FATAL_ERROR "ERROR: could not find uuid package" )
	else()
	    find_library( uuid_lib libossp-uuid.so )
	    if( NOT uuid_lib )
		    message( FATAL_ERROR "ERROR: could not find libossp-uuid.so" )
	    else()
		    message( STATUS "Found version \"${UUID-OSSP_VERSION}\" of ossp-uuid library: ${uuid_lib}" )
	    endif()
	endif()
else( )
	find_library( uuid_lib libuuid.so )
	if( NOT uuid_lib )
	        message( FATAL_ERROR "ERROR: could not find libuuid.so" )
	else()
		message( STATUS "Found version \"${UUID_VERSION}\" of uuid library: ${uuid_lib}" )
	endif()
endif()

if( NOT BLKID_FOUND )
	message( FATAL_ERROR "ERROR: could not find blkid package" )
else( NOT BLKID_FOUND )
	#add_definitions( "${BLKID_CFLAGS}" )
	find_library( blkid_lib libblkid.so )
	if( blkid_lib )
		set( blkid "${blkid_lib}" )
		message( STATUS "Found version \"${BLKID_VERSION}\" of blkid library: ${blkid_lib}" )
	else( blkid_lib )
		message( FATAL_ERROR "ERROR: could not find libblkid.so" )
	endif( blkid_lib )
endif( NOT BLKID_FOUND )

if( NOT CRYPTSETUP_FOUND )
	message( FATAL_ERROR "ERROR: could not find cryptsetup package" )
else( NOT CRYPTSETUP_FOUND )
	#add_definitions( "${CRYPTSETUP_CFLAGS}" )
	find_library( cryptsetup_lib libcryptsetup.so )
	#set( cryptsetup_lib "${CRYPTSETUP_LIBDIR}/libcryptsetup.so" )
	MESSAGE( STATUS "Found version \"${CRYPTSETUP_VERSION}\" of cryptsetup library: ${cryptsetup_lib}" )

	if( NOT cryptsetup_lib )
		message( FATAL_ERROR "ERROR: could not find libcryptsetup.so" )
	endif( NOT cryptsetup_lib )

endif( NOT CRYPTSETUP_FOUND )

find_file( GCRYPT_INCLUDE_FILE gcrypt.h )

find_library( GCRYPT_LIBRARY gcrypt )

if( NOT GCRYPT_INCLUDE_FILE )
	MESSAGE( FATAL_ERROR "ERROR: could not find gcrypt header file" )
else()
	MESSAGE( STATUS "Found gcrypt header file: ${GCRYPT_INCLUDE_FILE}" )
endif()

if( NOT GCRYPT_LIBRARY )
	MESSAGE( FATAL_ERROR "ERROR: could not find gcrypt library(libgcrypt.so)" )
else()
	MESSAGE( STATUS "Found gcrypt library: ${GCRYPT_LIBRARY}" )
endif()

if( WITH_PWQUALITY )
	pkg_check_modules( PWQUALITY pwquality )
endif( WITH_PWQUALITY )

file( WRITE  ${PROJECT_BINARY_DIR}/install_prefix.h "\n#define INSTALL_PREFIX \"${CMAKE_INSTALL_PREFIX}/\"\n" )

if( REUSEMOUNTPOINT )
	file( WRITE  ${PROJECT_BINARY_DIR}/reuse_mount_point.h "\n#define REUSE_MOUNT_POINT 1\n" )
else()
	file( WRITE  ${PROJECT_BINARY_DIR}/reuse_mount_point.h "\n#define REUSE_MOUNT_POINT 0\n" )
endif()

set( PLUGINPATH "${CMAKE_INSTALL_FULL_LIBDIR}/zuluCrypt" )

file( WRITE ${PROJECT_BINARY_DIR}/plugin_path.h "\n#define ZULUCRYPTpluginPath      \"${PLUGINPATH}/\"\n" )
file( APPEND ${PROJECT_BINARY_DIR}/plugin_path.h "\n#define ZULUCRYPTTestPlugin      \"${PLUGINPATH}/zuluCrypt-testKey\"\n" )

set( BUILD_TCPLAY "true" )
set( TCPLAY_NEW_API "true" )

if( NOT DEFINED INTERNAL_ZULUPLAY )
	set( INTERNAL_ZULUPLAY "false" )
endif()

if( INTERNAL_ZULUPLAY )
	message( STATUS "---------------------------------------------------------------------------" )
	message( STATUS "Internal version of zuluplay will be used to create TrueCrypt and VeraCrypt volumes." )
	message( STATUS "Internal version of zuluplay will be used to create and restore TrueCrypt and VeraCrypt volume headers." )
	message( STATUS "Unlocking of TrueCrypt and VeraCrypt volumes will be done by cryptsetup." )
	message( STATUS "Creation and restoration of VeraCrypt volume headers is currently disabled." )
	message( STATUS "---------------------------------------------------------------------------" )

	set( STATIC_ZULUPLAY "true" )
else()
	pkg_check_modules( ZULUPLAY zuluplay )

	if( ZULUPLAY_FOUND )

	    if( ZULUPLAY_VERSION VERSION_GREATER "1.2" )

		        include_directories( ${ZULUPLAY_INCLUDEDIR} )
			link_directories( ${ZULUPLAY_LIBDIR} )

			message( STATUS "---------------------------------------------------------------------------" )
			message( STATUS "External version of zuluplay will be used to create TrueCrypt and VeraCrypt volumes." )
			message( STATUS "External version of zuluplay will be used to create and restore TrueCrypt volume headers." )
			message( STATUS "Unlocking of TrueCrypt and VeraCrypt volumes will be done by cryptsetup." )
			message( STATUS "Creation and restoration of VeraCrypt volume headers is currently disabled." )
			message( STATUS "---------------------------------------------------------------------------" )

			set( STATIC_ZULUPLAY "false" )
	    else()
		        message( FATAL_ERROR "---------------------------------------------------------------------------\nInstalled version of zuluplay is too old(<1.3)\nPlease install a more receant version from: https://github.com/mhogomchungu/zuluplay\n---------------------------------------------------------------------------" )
	    endif()
	else()
		message( STATUS "---------------------------------------------------------------------------" )
		message( STATUS "Internal version of zuluplay will be used to create TrueCrypt and VeraCrypt volumes." )
		message( STATUS "Internal version of zuluplay will be used to create and restore TrueCrypt volume headers." )
		message( STATUS "Unlocking of TrueCrypt and VeraCrypt volumes will be done by cryptsetup." )
		message( STATUS "Creation and restoration of VeraCrypt volume headers is currently disabled." )
		message( STATUS "---------------------------------------------------------------------------" )

		set( STATIC_ZULUPLAY "true" )
	endif()
endif()

if( STATIC_ZULUPLAY )
	set( ZULUPLAY_STATIC_ONLY "true" )
	ADD_SUBDIRECTORY( ${PROJECT_SOURCE_DIR}/external_libraries/tcplay )
	include_directories( ${PROJECT_SOURCE_DIR}/external_libraries/tcplay )
endif()

if( SHARE_MOUNT_PREFIX )
    if( "${SHARE_MOUNT_PREFIX}" STREQUAL "default" )
	file( WRITE  ${PROJECT_BINARY_DIR}/share_mount_prefix_path.h "\n#define SHARE_MOUNT_PREFIX \"/run/media/public\"\n" )
    elseif( "${SHARE_MOUNT_PREFIX}" STREQUAL "" )
	file( WRITE  ${PROJECT_BINARY_DIR}/share_mount_prefix_path.h "\n#define SHARE_MOUNT_PREFIX \"/run/media/public\"\n" )
    else()
	file( WRITE  ${PROJECT_BINARY_DIR}/share_mount_prefix_path.h "\n#define SHARE_MOUNT_PREFIX \"${SHARE_MOUNT_PREFIX}\"\n" )
    endif()
else()
	file( WRITE  ${PROJECT_BINARY_DIR}/share_mount_prefix_path.h "\n#define SHARE_MOUNT_PREFIX \"/run/media/public\"\n" )
endif()

if( HOMEMOUNTPREFIX )
	file( WRITE  ${PROJECT_BINARY_DIR}/mount_prefix_path.h "\n#define USE_HOME_PATH_AS_MOUNT_PREFIX 1\n" )
else()
	file( WRITE  ${PROJECT_BINARY_DIR}/mount_prefix_path.h "\n#define USE_HOME_PATH_AS_MOUNT_PREFIX 0\n" )
endif()

#add_library( crypt_buffer STATIC plugins/network_key/crypt_buffer.c zuluCrypt-cli/utility/socket/socket.c )
#set_target_properties( crypt_buffer PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pthread  -pedantic " )
#TARGET_LINK_LIBRARIES( crypt_buffer -lgcrypt )

ADD_SUBDIRECTORY( zuluCrypt-cli )

if( NOT DEFINED INTERNAL_LXQT_WALLET )
	set( INTERNAL_LXQT_WALLET "false" )
endif()

add_subdirectory( zuluSafe )

if( NOT NOGUI )

	option(BUILD_WITH_QT6 "Build with Qt 6" OFF)

	if(BUILD_WITH_QT6)
		find_package(Qt6 COMPONENTS Core Widgets Network REQUIRED )
		message("-- building with Qt6")
		set(BUILD_WITH_QT6 ON)

		set(CMAKE_INCLUDE_CURRENT_DIR ON)

		set(CMAKE_AUTOMOC ON)
		set(CMAKE_AUTORCC ON)
		set(CMAKE_AUTOUIC ON)

		message( STATUS "Found Qt6Widgets, version  ${Qt6Widgets_VERSION}" )
		message( STATUS "Found Qt6Core, version  ${Qt6Core_VERSION}" )
		message( STATUS "Found Qt6Network, version  ${Qt6Network_VERSION}" )

		set( CMAKE_INCLUDE_CURRENT_DIR ON )
		include_directories( ${Qt6Widgets_INCLUDE_DIRS} ${Qt6Network_INCLUDE_DIRS} ${Qt6Core_INCLUDE_DIRS} )

		add_definitions( ${Qt6Widgets_DEFINITIONS} )
	else()
		find_package(Qt5 COMPONENTS Core QUIET)
		if(Qt5_FOUND)
			message("-- building with Qt5")
			set(BUILD_WITH_QT6 OFF)
			find_package(Qt5 COMPONENTS Core Widgets Network REQUIRED)

			message( STATUS "Found Qt5Widgets, version  ${Qt5Widgets_VERSION}" )
			message( STATUS "Found Qt5Core, version  ${Qt5Core_VERSION}" )
			message( STATUS "Found Qt5Network, version  ${Qt5Network_VERSION}" )

			set( CMAKE_INCLUDE_CURRENT_DIR ON )
			include_directories( ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS} ${Qt5Core_INCLUDE_DIRS} )

			add_definitions( ${Qt5Widgets_DEFINITIONS} )
		else()
			find_package(Qt6 COMPONENTS Core QUIET)

			if(Qt6_FOUND)
				message("-- building with Qt6")
				set(BUILD_WITH_QT6 ON)
				find_package(Qt6 COMPONENTS Core Widgets Network REQUIRED)

				set(CMAKE_INCLUDE_CURRENT_DIR ON)

				set(CMAKE_AUTOMOC ON)
				set(CMAKE_AUTORCC ON)
				set(CMAKE_AUTOUIC ON)

				message( STATUS "Found Qt6Widgets, version  ${Qt6Widgets_VERSION}" )
				message( STATUS "Found Qt6Core, version  ${Qt6Core_VERSION}" )
				message( STATUS "Found Qt6Network, version  ${Qt6Network_VERSION}" )

				set( CMAKE_INCLUDE_CURRENT_DIR ON )
				include_directories( ${Qt6Widgets_INCLUDE_DIRS} ${Qt6Network_INCLUDE_DIRS} ${Qt6Core_INCLUDE_DIRS} )

				add_definitions( ${Qt6Widgets_DEFINITIONS} )
			else()
			        message( FATAL_ERROR "-- ERROR: Failed to find Qt5 or Qt6")
			endif()
		endif()
	endif()

	if( INTERNAL_LXQT_WALLET )

		message( STATUS "---------------------------------------------------------------------------" )
		message( STATUS "lxqt wallet support will be provided by an internal library." )
		message( STATUS "---------------------------------------------------------------------------" )

		include_directories( external_libraries/lxqt_wallet )
		include_directories( external_libraries/lxqt_wallet/frontend )
		include_directories( external_libraries/lxqt_wallet/backend )

		include_directories( ${PROJECT_BINARY_DIR}/external_libraries/lxqt_wallet/lxqt_wallet )
		include_directories( ${PROJECT_BINARY_DIR}/external_libraries/lxqt_wallet/lxqt_wallet/frontend )
		include_directories( ${PROJECT_BINARY_DIR}/external_libraries/lxqt_wallet/lxqt_wallet/backend )

		add_subdirectory( external_libraries/lxqt_wallet )
	else()
		pkg_check_modules( LXQT_WALLET lxqt-wallet )

		if( NOT LXQT_WALLET_FOUND )

			message( STATUS "ERROR: could not find lxqt wallet package" )
			message( STATUS "---------------------------------------------------------------------------" )
			message( STATUS "lxqt wallet support will be provided by an internal library." )
			message( STATUS "---------------------------------------------------------------------------" )

			include_directories( external_libraries/lxqt_wallet )
			include_directories( external_libraries/lxqt_wallet/frontend )
			include_directories( external_libraries/lxqt_wallet/backend )

			include_directories( ${PROJECT_BINARY_DIR}/external_libraries/lxqt_wallet )
			include_directories( ${PROJECT_BINARY_DIR}/external_libraries/lxqt_wallet/frontend )
			include_directories( ${PROJECT_BINARY_DIR}/external_libraries/lxqt_wallet/backend )

			add_subdirectory( external_libraries/lxqt_wallet )
		else()
			if( BUILD_WITH_QT6 )

				if( LXQT_WALLET_VERSION VERSION_LESS "4.0.2" )

					include_directories( external_libraries/lxqt_wallet )
					include_directories( external_libraries/lxqt_wallet/frontend )
					include_directories( external_libraries/lxqt_wallet/backend )

					include_directories( ${PROJECT_BINARY_DIR}/external_libraries/lxqt_wallet/lxqt_wallet )
					include_directories( ${PROJECT_BINARY_DIR}/external_libraries/lxqt_wallet/lxqt_wallet/frontend )
					include_directories( ${PROJECT_BINARY_DIR}/external_libraries/lxqt_wallet/lxqt_wallet/backend )

					add_subdirectory( external_libraries/lxqt_wallet )

					message( STATUS "------------------------------------------------------------------------------" )
					message( STATUS "WARNING: Qt6 build requires lxqt-wallet >= 4.0.2, switching to bundled version" )
					message( STATUS "------------------------------------------------------------------------------" )
				else()
				        # lxqt-wallet 4.0.0 renamed a header from "lxqtwallet.h" to "lxqt-wallet.h",
					# so this is needed to choose the right header to include when compiling.

					if( LXQT_WALLET_VERSION VERSION_GREATER "4.0.0" )
					        add_compile_definitions( LXQT_WALLET_HYPHEN_HEADER )
					endif()

					include_directories( ${LXQT_WALLET_INCLUDEDIR} )
					link_directories( ${LXQT_WALLET_LIBDIR} )

					message( STATUS "---------------------------------------------------------------------------" )
					message( STATUS "lxqt wallet support will be provided by an external libary." )
					message( STATUS "---------------------------------------------------------------------------" )
				endif()
			else()
				if( LXQT_WALLET_VERSION VERSION_GREATER "3.1.0"  )

					include_directories( ${LXQT_WALLET_INCLUDEDIR} )
					link_directories( ${LXQT_WALLET_LIBDIR} )

					message( STATUS "---------------------------------------------------------------------------" )
					message( STATUS "lxqt wallet support will be provided by an external libary." )
					message( STATUS "---------------------------------------------------------------------------" )
				else()
					include_directories( external_libraries/lxqt_wallet )
					include_directories( external_libraries/lxqt_wallet/frontend )
					include_directories( external_libraries/lxqt_wallet/backend )

					include_directories( ${PROJECT_BINARY_DIR}/external_libraries/lxqt_wallet/lxqt_wallet )
					include_directories( ${PROJECT_BINARY_DIR}/external_libraries/lxqt_wallet/lxqt_wallet/frontend )
					include_directories( ${PROJECT_BINARY_DIR}/external_libraries/lxqt_wallet/lxqt_wallet/backend )

					add_subdirectory( external_libraries/lxqt_wallet )

					message( STATUS "---------------------------------------------------------------------------" )
					message( STATUS "lxqt wallet support will be provided by an internal libary." )
					message( STATUS "---------------------------------------------------------------------------" )
				endif()
			endif()
		endif()
	endif()

	if( WITH_PWQUALITY AND PWQUALITY_FOUND )
		find_file ( header_pwquality pwquality.h PATHS ${PWQUALITY_INCLUDE_DIRS} )
		find_library ( library_pwquality libpwquality.so )
		set( BUILDPWQUALITY "true" )
		if( library_pwquality )
			if( header_pwquality )
				file( WRITE ${PROJECT_BINARY_DIR}/can_build_pwquality.h "#define BUILD_PWQUALITY 1\n" )
				file( APPEND ${PROJECT_BINARY_DIR}/can_build_pwquality.h "extern \"C\"\n{\n#include <${header_pwquality}>\n}" )
			else( header_pwquality )
				file( WRITE ${PROJECT_BINARY_DIR}/can_build_pwquality.h "#define BUILD_PWQUALITY 0\n" )
			endif( header_pwquality )
		else( library_pwquality )
			file( WRITE ${PROJECT_BINARY_DIR}/can_build_pwquality.h "#define BUILD_PWQUALITY 0\n" )
		endif( library_pwquality )
	else( WITH_PWQUALITY AND PWQUALITY_FOUND )
		file( WRITE ${PROJECT_BINARY_DIR}/can_build_pwquality.h "#define BUILD_PWQUALITY 0\n" )
	endif( WITH_PWQUALITY AND PWQUALITY_FOUND )

	if( MCHUNGU_TASK_FOUND )

		message( STATUS "---------------------------------------------------------------------------" )
		message( STATUS "mhogomchungu_task library will be provided by an external library." )
		message( STATUS "---------------------------------------------------------------------------" )

		include_directories( "${MCHUNGU_TASK_INCLUDEDIR}" )
		link_directories( "${MCHUNGU_TASK_LIBDIR}" )
	else()
		message( STATUS "---------------------------------------------------------------------------" )
		message( STATUS "mhogomchungu_task library will be provided by an internal library." )
		message( STATUS "---------------------------------------------------------------------------" )
		include_directories( ${PROJECT_SOURCE_DIR}/external_libraries/tasks )
		ADD_SUBDIRECTORY( ${PROJECT_SOURCE_DIR}/external_libraries/tasks )
	endif()

	if( CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 10.0.0)
		set( CMAKE_CXX_STANDARD 14 )
		MESSAGE( STATUS "Setting C++ version to C++14" )
	else()
		set( CMAKE_CXX_STANDARD 14 )
		MESSAGE( STATUS "Setting C++ version to C++14" )
	endif()

	set( CMAKE_CXX_STANDARD_REQUIRED ON )
	set( CMAKE_CXX_EXTENSIONS OFF)

	ADD_SUBDIRECTORY( zuluCrypt-gui/sharedObjects )
	ADD_SUBDIRECTORY( zuluCrypt-gui )
	ADD_SUBDIRECTORY( plugins )
	ADD_SUBDIRECTORY( zuluMount-gui )

	file( WRITE ${PROJECT_BINARY_DIR}/zuluPolkit.h "#define POLKIT_SUPPORT 1" )

	if( USE_POLKIT )

		file( APPEND ${PROJECT_BINARY_DIR}/zuluPolkit.h "\n#define AUTO_ENABLE_POLKIT_SUPPORT 1" )
	else()
		file( APPEND ${PROJECT_BINARY_DIR}/zuluPolkit.h "\n#define AUTO_ENABLE_POLKIT_SUPPORT 0" )
	endif()

	ADD_SUBDIRECTORY( zuluPolkit )
endif( NOT NOGUI )

file( WRITE ${PROJECT_BINARY_DIR}/locale_path.h "\n#define TRANSLATION_PATH \"${CMAKE_INSTALL_FULL_DATADIR}/zuluCrypt/translations/\"\n")

message( STATUS "---------------------------------------------------------------------------" )
message( STATUS "Optional functionality that will be build:" )

if( NOT NOGUI )
	message( STATUS "GUI front ends, zuluCrypt-gui and zuluMount-gui" )
endif( NOT NOGUI )

if( BUILDPWQUALITY )
	message( STATUS "pwquality library support to give passphrase strength measure" )
endif( BUILDPWQUALITY )

if( UDEVSUPPORT )
	message( STATUS "udev support will be enabled" )
endif( UDEVSUPPORT )

if( BUILD_TCPLAY )
	message( STATUS "zuluplay adds the ability to create TrueCrypt compatible encrypted containers" )
endif( BUILD_TCPLAY )

if( CRYPTSETUP_VERSION VERSION_GREATER "1.6.3" )
	file( WRITE ${PROJECT_BINARY_DIR}/support_whirlpool.h "\n#define SUPPORT_WHIRLPOOL 1\n" )
else()
	file( WRITE ${PROJECT_BINARY_DIR}/support_whirlpool.h "\n#define SUPPORT_WHIRLPOOL 0\n" )
endif()

if( ( CRYPTSETUP_VERSION VERSION_GREATER "2.0.0" ) OR ( CRYPTSETUP_VERSION VERSION_EQUAL "2.0.0" ) )
	file( WRITE ${PROJECT_BINARY_DIR}/veracrypt_pim.h "\n#define SUPPORT_VERACRYPT_PIM 1\n" )
else()
	file( WRITE ${PROJECT_BINARY_DIR}/veracrypt_pim.h "\n#define SUPPORT_VERACRYPT_PIM 0\n" )

endif()

if( ( CRYPTSETUP_VERSION VERSION_GREATER "2.0.3" ) OR ( CRYPTSETUP_VERSION VERSION_EQUAL "2.0.3" ) )
	file( WRITE ${PROJECT_BINARY_DIR}/luks2_support.h "\n#define SUPPORT_crypt_get_pbkdf_default 1\n" )
else()
	file( WRITE ${PROJECT_BINARY_DIR}/luks2_support.h "\n#define SUPPORT_crypt_get_pbkdf_default 0\n" )
endif()

if( ( CRYPTSETUP_VERSION VERSION_GREATER "2.1.0" ) OR ( CRYPTSETUP_VERSION VERSION_EQUAL "2.1.0" ) )
	file( WRITE ${PROJECT_BINARY_DIR}/luks_slot_status.h "\n#define SUPPORT_crypt_keyslot_get_pbkdf 1\n" )
else()
	file( WRITE ${PROJECT_BINARY_DIR}/luks_slot_status.h "\n#define SUPPORT_crypt_keyslot_get_pbkdf 0\n" )
endif()

if( CRYPTSETUP_VERSION VERSION_GREATER "1.6.0" )
	file( WRITE ${PROJECT_BINARY_DIR}/check_tcrypt.h "\n#define CHECK_TCRYPT 1\n" )
else()
	file( WRITE ${PROJECT_BINARY_DIR}/check_tcrypt.h "\n#define CHECK_TCRYPT 0\n" )
endif()

message( STATUS "---------------------------------------------------------------------------" )
message( STATUS "---------------------------------------------------------------------------" )
message( STATUS "Optional functionality that will NOT be build:" )

if( NOGUI )
	message( STATUS "GUI front ends, zuluCrypt-gui and zuluMount-gui\n" )
endif( NOGUI )

if( NOT BUILDPWQUALITY )
	message( STATUS "pwquality library used to measure passphrase strength" )
	message( STATUS "Sources can be found at https://fedorahosted.org/libpwquality/\n" )
endif( NOT BUILDPWQUALITY )

if( NOT UDEVSUPPORT )
	message( STATUS "udev support\n" )
endif( NOT UDEVSUPPORT )

message( STATUS "---------------------------------------------------------------------------" )
message( STATUS "\n" )

if( CRYPTSETUP_VERSION VERSION_GREATER "1.4.0" )
	file( WRITE ${PROJECT_BINARY_DIR}/luks_external_header.h "\n#define LUKS_EXTERNAL_HEADER 1\n" )
else( CRYPTSETUP_VERSION VERSION_GREATER "1.4.0" )
	file( WRITE ${PROJECT_BINARY_DIR}/luks_external_header.h "\n#define LUKS_EXTERNAL_HEADER 0\n" )
endif( CRYPTSETUP_VERSION VERSION_GREATER "1.4.0" )

if( UDEVSUPPORT )

	file( WRITE ${PROJECT_BINARY_DIR}/udev_support.h "\n#define UDEV_SUPPORT 1\n")

	message( STATUS "udev will be consulted when deciding if a volume is system or not." )
	message( STATUS "Please read \"udev_support\" file for more information about udev support and associated problems.\n" )

else( UDEVSUPPORT )

	file( WRITE ${PROJECT_BINARY_DIR}/udev_support.h "\n#define UDEV_SUPPORT 0\n")

	message( STATUS "udev will NOT be consulted when deciding if a volume is system or not." )
	message( STATUS "Please read \"udev_support\" file for more information about udev support and how to enable it." )
	message( STATUS "It is probably a good idea, to enable it if you are creating a package for distribution.\n" )

endif( UDEVSUPPORT )

if( NOT NOGUI )
	if( BUILD_WITH_QT6 )
		message( STATUS "---------------------------------------------------------------------------" )
		message( STATUS "Building GUI components using Qt6" )
		message( STATUS "---------------------------------------------------------------------------\n\n" )
	else()
		message( STATUS "---------------------------------------------------------------------------" )
		message( STATUS "Building GUI components using Qt5" )
		message( STATUS "---------------------------------------------------------------------------\n\n" )
	endif()
else()
	message( STATUS "---------------------------------------------------------------------------" )
	message( STATUS "Not building GUI components" )
	message( STATUS "---------------------------------------------------------------------------\n\n" )
endif()

install ( FILES zuluCrypt.xml  DESTINATION "${CMAKE_INSTALL_PREFIX}/share/mime/packages/" )

install ( FILES zuluCrypt-cli.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1 )
install ( FILES zuluCrypt-gui.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1 )
install ( FILES zuluMount-cli.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1 )
install ( FILES zuluMount-gui.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1 )
install ( FILES zuluSafe-cli.1  DESTINATION ${CMAKE_INSTALL_MANDIR}/man1 )
install ( FILES docs/zuluCrypt.pdf  DESTINATION ${CMAKE_INSTALL_DOCDIR} )

file( WRITE ${PROJECT_BINARY_DIR}/pdf_path.h "\n#define PDF_PATH \"${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DOCDIR}/zuluCrypt.pdf\"\n" )

install ( FILES translations/zuluCrypt/de_DE.qm DESTINATION ${CMAKE_INSTALL_DATADIR}/zuluCrypt/translations/zuluCrypt-gui )
install ( FILES translations/zuluCrypt/fr_FR.qm DESTINATION ${CMAKE_INSTALL_DATADIR}/zuluCrypt/translations/zuluCrypt-gui )
install ( FILES translations/zuluCrypt/en_US.qm DESTINATION ${CMAKE_INSTALL_DATADIR}/zuluCrypt/translations/zuluCrypt-gui )
install ( FILES translations/zuluCrypt/ar_SA.qm DESTINATION ${CMAKE_INSTALL_DATADIR}/zuluCrypt/translations/zuluCrypt-gui )

install ( FILES translations/zuluMount/de_DE.qm DESTINATION ${CMAKE_INSTALL_DATADIR}/zuluCrypt/translations/zuluMount-gui )
install ( FILES translations/zuluMount/en_US.qm DESTINATION ${CMAKE_INSTALL_DATADIR}/zuluCrypt/translations/zuluMount-gui )
install ( FILES translations/zuluMount/fr_FR.qm DESTINATION ${CMAKE_INSTALL_DATADIR}/zuluCrypt/translations/zuluMount-gui )
install ( FILES translations/zuluMount/ar_SA.qm DESTINATION ${CMAKE_INSTALL_DATADIR}/zuluCrypt/translations/zuluMount-gui )

#install ( FILES translations/zuluMount/de_DE.qm DESTINATION ${CMAKE_INSTALL_DATADIR}/zuluCrypt/translations/zuluMount-gui )
