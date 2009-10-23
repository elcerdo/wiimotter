CONFIG += qt debug link_pkgconfig
PKGCONFIG += bluez
LIBS += -lcwiid -lqwt
SOURCES += MainWindow.cpp ConnectThread.cpp Wiimote.cpp
HEADERS += MainWindow.h   ConnectThread.h   Wiimote.h
SOURCES += main.cpp
