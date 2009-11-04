#ifndef __WIIMOTE_H__
#define __WIIMOTE_H__

#include <QObject>
#include <cwiid.h>
#include <bluetooth/bluetooth.h>

class Wiimote: public QObject {
Q_OBJECT
friend void cwiid_msg_callback(cwiid_wiimote_t *wiimote, int mesg_count, cwiid_mesg mesg[], timespec *timestamp);
public:
    Wiimote(bdaddr_t addr,QObject *parent = NULL);
    ~Wiimote();
    QString getAddress(void) const;
    cwiid_state getState() const;
signals:
    void buttonToggled(int button,bool state);
protected:
    void parseButtons(uint16_t);
    uint16_t buttons_old;
    bdaddr_t addr;
};

#endif
