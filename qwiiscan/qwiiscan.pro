CONFIG += qt debug link_pkgconfig
PKGCONFIG += bluez liblo
LIBS += -lcwiid -lqwt
SOURCES += MainWindow.cpp ConnectThread.cpp Wiimote.cpp OscObject.cpp
HEADERS += MainWindow.h   ConnectThread.h   Wiimote.h	OscObject.h
SOURCES += main.cpp
