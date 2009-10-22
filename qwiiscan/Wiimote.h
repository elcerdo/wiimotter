#ifndef __WIIMOTE_H__
#define __WIIMOTE_H__

#include <QObject>
#include <bluetooth/bluetooth.h>

class Wiimote: public QObject {
Q_OBJECT
public:
    Wiimote(bdaddr_t addr,QObject *parent = NULL);
    ~Wiimote();
    QString getAddress(void) const;
protected:
    bdaddr_t addr;
};

#endif
