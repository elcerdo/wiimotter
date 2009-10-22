#ifndef __CONNECTTHREAD_H__
#define __CONNECTTHREAD_H__

#include <QThread>
#include "Wiimote.h"

class ConnectThread: public QThread {
Q_OBJECT
public:
    ConnectThread(QObject *parent=NULL);
    Wiimote *wiimote;
protected:
    virtual void run();
};

#endif
