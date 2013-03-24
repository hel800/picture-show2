# ------------------------------------------------------------------
# picture-show2.pro is part of picture-show2.
# .....................................................................
#
# picture-show is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# picture-show2 is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with picture-show. If not, see <http://www.gnu.org/licenses/>.
#
# ......................................................................
#
# author: Sebastian Schäfer
# February 2013
#
# --------------------------------------------------------------------

# Add more folders to ship with the application, here
folder_01.source = qml/picture-show2
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

QT += widgets opengl qml quick

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    settingsdialog.cpp \
    supervisor.cpp \
    imageprovider.cpp \
    loaddirectory.cpp \
    iptcinfo.cpp \
    exif.cpp \
    helpwindow.cpp \
    readdirlist.cpp \
    loadcollectiondialog.cpp

# Please do not modify the following two lines. Required for deployment.
include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
qtcAddDeployment()

RESOURCES += \
    resources.qrc

RC_FILE = picture-show_icon.rc

TRANSLATIONS = picture-show2_en.ts

HEADERS += \
    settingsdialog.h \
    global.h \
    supervisor.h \
    imageprovider.h \
    readExif.h \
    loaddirectory.h \
    iptcinfo.h \
    exif.h \
    helpwindow.h \
    readdirlist.h \
    loadcollectiondialog.h

FORMS += \
    settingsdialog.ui \
    helpwindow.ui \
    savecolldialog.ui \
    loadcollectiondialog.ui
