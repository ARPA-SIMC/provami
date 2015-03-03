# From the qantenna .pro files: http://qantenna.sourceforge.net/
unix {
    CONFIG += debug
    # Prefix: base instalation directory
    isEmpty(PREFIX){
        PREFIX = /usr/local
    }

    DEB_BUILD = $$system(echo \$DEB_BUILD_OPTIONS)
    contains(DEB_BUILD, nostrip){
        QMAKE_STRIP =:
    }

    # No point defining this, since deb/rpm build tools need to redefine PREFIX
    # at build time to install in the package build dirs.
    #DEFINES += PREFIX=\\\"$${PREFIX}\\\"
    #DEFINES += DATADIR=\\\"$$DATADIR\\\" PKGDATADIR=\\\"$$PKGDATADIR\\\"

    DEFINES += DATADIR=\\\"$$PREFIX/share/provami\\\"
}
