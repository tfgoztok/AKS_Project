QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

SOURCES += \
    src/ClickableMissileWidget.cpp \
    src/Logger.cpp \
    src/main.cpp \
    src/AKSApp.cpp \
    src/AKSGUI.cpp \
    src/CommunicationManager.cpp \
    src/AKSCore.cpp \
    src/ConfigReader.cpp

HEADERS += \
    src/AKSApp.h \
    src/AKSGUI.h \
    src/ClickableMissileWidget.h \
    src/CommunicationManager.h \
    src/AKSCore.h \
    src/ConfigReader.h \
    src/GlobalConstants.h \
    src/Logger.h \

FORMS += \
    ui/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target