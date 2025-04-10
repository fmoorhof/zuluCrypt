if(${CMAKE_VERSION} VERSION_GREATER "3.26")
    cmake_minimum_required(VERSION 3.26.0)
else()
    cmake_minimum_required(VERSION 3.0.2)
endif()

add_definitions( -D_FILE_OFFSET_BITS=64 -Wextra -Wall -pedantic -I${PROJECT_BINARY_DIR}/zuluCrypt-gui/ )

include_directories( ${PROJECT_BINARY_DIR}/zuluCrypt-gui/ )

set( UI_FILES createfile.ui
		about.ui
		help.ui
		createvolume.ui
		luksdeletekey.ui
		password.ui
		zulucrypt.ui
		createkeyfile.ui
		luksaddkey.ui
		createvolume.ui
		cryptoinfo.ui
		warnwhenextendingcontainerfile.ui
		createvolumeinexistingfile.ui
		managevolumeheader.ui
		cryptfiles.ui
		createvolumedialog.ui
		managesystemvolumes.ui
		erasedevice.ui
		help.ui
		filemanager.ui
		showluksslots.ui
)

set( zuluCryptMOC_FILES
		cryptoinfo.h
		about.h
		createfile.h
		luksaddkey.h
		createkeyfile.h
		warnwhenextendingcontainerfile.h
		luksdeletekey.h
		createvolume.h
		createvolumeinexistingfile.h
		password_dialog.h
		zulucrypt.h
		help.h
		erasedevice.h
		managevolumeheader.h
		cryptfiles.h
		createvolumedialog.h
		managesystemvolumes.h
		about.h
		help.h
		filemanager.h
		showluksslots.h
 )

set( SRC
	about.cpp
	secrets.cpp
	createfile.cpp
	createvolume.cpp
	luksdeletekey.cpp
	createkeyfile.cpp
	luksaddkey.cpp
	main.cpp
	warnwhenextendingcontainerfile.cpp
	password_dialog.cpp
	createvolumeinexistingfile.cpp
	zulucrypt.cpp
	cryptoinfo.cpp
	erasedevice.cpp
	managevolumeheader.cpp
	cryptfiles.cpp
	createvolumedialog.cpp
	managesystemvolumes.cpp
	keystrength.cpp
	help.cpp
	about.cpp
	filemanager.cpp
	showluksslots.cpp
)

if(BUILD_WITH_QT6)

        add_executable( zuluCrypt-gui ${zuluCryptMOC_FILES} ${UI_FILES} ${SRC} icon.qrc  )

	#set_target_properties( zuluCrypt-gui PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}" )

	set_target_properties( zuluCrypt-gui PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pedantic" )

	if( WITH_PWQUALITY AND library_pwquality )
	        TARGET_LINK_LIBRARIES( zuluCrypt-gui lxqt-wallet sharedObject zuluCryptPluginManager ${Qt6Widgets_LIBRARIES} ${Qt6Core_LIBRARIES} ${Qt6Network_LIBRARIES} ${blkid} ${library_pwquality} )
	else()
	        TARGET_LINK_LIBRARIES( zuluCrypt-gui lxqt-wallet sharedObject zuluCryptPluginManager ${Qt6Widgets_LIBRARIES} ${Qt6Core_LIBRARIES} ${Qt6Network_LIBRARIES} ${blkid} ${kwallet_library} )
	endif()
else()
        INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

	Qt5_WRAP_UI( UI ${UI_FILES} )
	Qt5_WRAP_CPP( MOC ${zuluCryptMOC_FILES} )
	Qt5_ADD_RESOURCES( TRAY_RC_SRCS icon.qrc )

	add_executable( zuluCrypt-gui ${MOC} ${UI} ${SRC} ${TRAY_RC_SRCS}  )

	#set_target_properties( zuluCrypt-gui PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}" )

	set_target_properties( zuluCrypt-gui PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pedantic" )

	if( WITH_PWQUALITY AND library_pwquality )
	        TARGET_LINK_LIBRARIES( zuluCrypt-gui lxqt-wallet sharedObject zuluCryptPluginManager ${Qt5Widgets_LIBRARIES} ${Qt5Core_LIBRARIES} ${Qt5Network_LIBRARIES} ${blkid} ${library_pwquality} )
	else()
	        TARGET_LINK_LIBRARIES( zuluCrypt-gui lxqt-wallet sharedObject zuluCryptPluginManager ${Qt5Widgets_LIBRARIES} ${Qt5Core_LIBRARIES} ${Qt5Network_LIBRARIES} ${blkid} ${kwallet_library} )
	endif()
endif()

install( TARGETS zuluCrypt-gui RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} )

install( FILES ${PROJECT_BINARY_DIR}/zuluCrypt.desktop
DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications
)

install( FILES zuluCrypt.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/apps )
install( FILES zuluCrypt.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons )
install( FILES zuluCrypt.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pixmaps )
install( FILES zuluCrypt.nicolas.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/apps )
install( FILES zuluCrypt.nicolas.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pixmaps )
install( FILES zuluCrypt.papirus.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/apps )
install( FILES zuluCrypt.papirus.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pixmaps )
install( FILES zuluCrypt.papirus.dark.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/apps )
install( FILES zuluCrypt.papirus.dark.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pixmaps )
#install (FILES zuluCrypt.png DESTINATION share/icons/hicolor/32x32/apps)

# desktop file section

file( WRITE ${PROJECT_BINARY_DIR}/zuluCrypt.desktop

"[Desktop Entry]
Comment[en_US]=
Comment=
Exec=${CMAKE_INSTALL_FULL_BINDIR}/zuluCrypt-gui -d %U
GenericName[en_US]=Encrypted volumes manager
GenericName=Encrypted volumes manager
Icon=zuluCrypt
Name[en_US]=ZuluCrypt
Name=ZuluCrypt
NoDisplay=false
StartupNotify=true
Terminal=false
Type=Application
MimeType=application/x-raw-disk-image;
Categories=Security;Utility;Qt;X-MandrivaLinux-System-FileTools;\n")
