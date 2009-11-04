#include "OscObject.h"

#include <lo/lo.h>

OscObject::OscObject(QObject *parent) : QObject(parent), prefix("/wiimote/"), ff(false), client(NULL) {
    client = lo_address_new("localhost","9000");
    Q_ASSERT(client);
}

void OscObject::forward(bool f) {
    ff = f;
    lo_send(client,qPrintable(prefix + "forward"),"i",ff);
}

void OscObject::forwardButton(int n,bool state) {
    if (not ff) return;
    lo_send(client,qPrintable(prefix + "button"),"ii",n,state);
}

void OscObject::forwardAccel(double ax,double ay,double az) {
    if (not ff) return;
    lo_send(client,qPrintable(prefix + "accel"),"fff",ax,ay,az);
}




