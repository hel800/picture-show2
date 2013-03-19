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
    readdirlist.cpp

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
    readdirlist.h

FORMS += \
    settingsdialog.ui \
    helpwindow.ui
