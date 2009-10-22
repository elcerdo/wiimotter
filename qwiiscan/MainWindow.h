#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <QMainWindow>
#include <QAction>
#include "ConnectThread.h"

class MainWindow: public QMainWindow {
Q_OBJECT
public:
    MainWindow(QWidget *parent=NULL);
protected slots:
    void wiiconnect(bool checked);
    void wiiconnected();
protected:
    QAction *wiiconnect_action;
    ConnectThread *wiiconnect_thread;
};

#endif
