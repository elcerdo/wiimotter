#ifndef __WIIMOTE_H__
#define __WIIMOTE_H__

#include <QObject>
#include <cwiid.h>
#include <bluetooth/bluetooth.h>

class Wiimote: public QObject {
Q_OBJECT
public:
    Wiimote(bdaddr_t addr,QObject *parent = NULL);
    ~Wiimote();
    QString getAddress(void) const;
    cwiid_state getState() const;
protected:
    bdaddr_t addr;
};

#endif
