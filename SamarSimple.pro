QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets sql

RC_ICONS = appico.ico

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    CustomModel.cpp \
    main.cpp \
    mainwindow.cpp \
    switch.cpp

HEADERS += \
    CustomModel.h \
    mainwindow.h \
    switch.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    SamarSimple_en_US.ts \
    SamarSimple_ar_EG.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    SamarSimple_ar_EG.ts \
    appico.ico

RESOURCES += \
    resources.qrc
