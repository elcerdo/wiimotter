CONFIG += qt debug link_pkgconfig
PKGCONFIG += bluez
LIBS += -lcwiid
SOURCES += MainWindow.cpp ConnectThread.cpp
HEADERS += MainWindow.h   ConnectThread.h
SOURCES += main.cpp
