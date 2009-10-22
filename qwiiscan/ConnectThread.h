#ifndef __CONNECTTHREAD_H__
#define __CONNECTTHREAD_H__

#include <QThread>
#include <bluetooth/bluetooth.h>

class ConnectThread: public QThread {
Q_OBJECT
public:
    ConnectThread(QObject *parent=NULL);
    bool found;
    bdaddr_t bdaddr;
protected:
    virtual void run();
};

#endif
