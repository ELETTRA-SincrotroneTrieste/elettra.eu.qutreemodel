# + ----------------------------------------------------------------- +
#
# Customization section:
#
# Customize the following paths according to your installation:
#
#
# Here qumbia-qtcontrols will be installed
# INSTALL_ROOT can be specified from the command line running qmake "INSTALL_ROOT=/my/install/path"
#

isEmpty(INSTALL_ROOT) {
    INSTALL_ROOT = /usr/local/cumbia-libs
}

isEmpty(prefix) {
    prefix = $${INSTALL_ROOT}
}

include ($${INSTALL_ROOT}/include/cumbia-qtcontrols/cumbia-qtcontrols.pri)

#
#
# Here qumbia-plugins include files will be installed
    QUMBIA_PLUGINS_INCLUDES=$${INSTALL_ROOT}/include/qumbia-plugins
#
#
# Here qumbia-plugins share files will be installed
#
    QUMBIA_PLUGINS_SHARE=$${INSTALL_ROOT}/share/qumbia-plugins
#
#
# Here qumbia-plugins libraries will be installed
    QUMBIA_PLUGINS_LIBDIR=$${INSTALL_ROOT}/lib/qumbia-plugins
#
#
# Here qumbia-plugins documentation will be installed
    QUMBIA_PLUGINS_DOCDIR=$${INSTALL_ROOT}/share/doc/qumbia-plugins
#
#
# # Plugin interface will be installed under cumbia-qtcontrols/include dirs
# CUMBIA_QTCONTROLS_INCLUDES is defined in cumbia-qtcontrols.pri, which is included above
#
# + ----------------------------------------------------------------- +
#


INC_DIR = $${QUMBIA_PLUGINS_INCLUDES}
SHAREDIR = $${QUMBIA_PLUGINS_SHARE}
PLUGIN_LIB_DIR = $${QUMBIA_PLUGINS_LIBDIR}

DESTDIR = plugins

VERSION_HEX = 0x010000
VERSION = 1.0.0
DEFINES += QUTREEMODEL_PLUGIN_VERSION_STR=\"\\\"$${VERSION}\\\"\" \
    QUTREEMODEL_PLUGIN_VERSION=$${VERSION_HEX}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

DEFINES += QT_NO_DEBUG_OUTPUT

DOC_DIR = $${QUMBIA_PLUGINS_DOCDIR}
doc.commands = \
    doxygen \
    Doxyfile;
doc.files = doc/*
doc.path = $${DOC_DIR}

QMAKE_EXTRA_TARGETS += doc

# lib

target.path = $${PLUGIN_LIB_DIR}
inc.path = $${INC_DIR}

# plugin interface, installed under CUMBIA_QTCONTROLS_INCLUDES
# defined in cumbia-qtcontrols.pri, by default $${INSTALL_ROOT}/include/cumbia-qtcontrols
#
iface_inc.path = $${CUMBIA_QTCONTROLS_INCLUDES}
iface_inc.files = qutreemodelplugininterface.h

# installation

INSTALLS += target \
    iface_inc \
    inc \
    doc

DISTFILES += \
    $$PWD/qumbia-plugins.md \
    CHANGELOG


TARGET = qutreemodel-plugin
TEMPLATE = lib
CONFIG += plugin debug

SOURCES += \
    qutreemodel.cpp \
    qutreeitem.cpp \
    qutreemodelplugin.cpp

HEADERS += \
    qutreeitem.h \
    qutreemodel.h \
    qutreemodelplugin.h \
    qutreemodelplugininterface.h

DISTFILES += qutreemodelplugin.json  \
    qutreemodelplugininterface.h

inc.files += $${HEADERS}

