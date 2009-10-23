#include "Wiimote.h"

#include <cstdio>

static cwiid_wiimote_t *instance = NULL;

void cwiid_err_callback(cwiid_wiimote_t *wiimote, const char *s, va_list ap) {
    int id = -1;
    if (wiimote) id = cwiid_get_id(wiimote);
    
    char foo[256];
    vsnprintf(foo,256,s,ap);
    qCritical("ERRRRRR %d: %s",id,foo);
}

void cwiid_msg_callback(cwiid_wiimote_t *wiimote, int mesg_count, cwiid_mesg mesg[], timespec *timestamp){
    Q_UNUSED(wiimote);
    Q_UNUSED(timestamp);
    for (int k=0; k<mesg_count; k++) switch (mesg[k].type) {
        case CWIID_MESG_BTN:
        //case CWIID_MESG_NUNCHUK:
            qDebug("message %d",mesg[k].type);
            break;
        default:
            break;
    }
}
 

Wiimote::Wiimote(bdaddr_t addr,QObject *parent) : QObject(parent), addr(addr) {
    Q_ASSERT(instance == NULL);
    
    qDebug("setting up cwiid");
    {
        cwiid_set_err(cwiid_err_callback);
        instance = cwiid_open(&addr,0);
        Q_ASSERT(instance);
        cwiid_set_mesg_callback(instance,cwiid_msg_callback);
        cwiid_enable(instance,CWIID_FLAG_MESG_IFC);
        cwiid_command(instance,CWIID_CMD_LED,CWIID_LED2_ON | CWIID_LED3_ON);
        //cwiid_command(instance,CWIID_CMD_RPT_MODE,CWIID_RPT_EXT | CWIID_RPT_BTN | CWIID_RPT_ACC | CWIID_RPT_STATUS | CWIID_RPT_IR);
        cwiid_command(instance,CWIID_CMD_RPT_MODE,CWIID_RPT_EXT | CWIID_RPT_BTN | CWIID_RPT_STATUS | CWIID_RPT_ACC);
    }
    
}

Wiimote::~Wiimote() {
    qDebug("closing cwiid");

    Q_ASSERT(instance);
    cwiid_disconnect(instance);
}

QString Wiimote::getAddress() const {
    char foo[19];
    ba2str(&addr,foo);
    return QString(foo);
}


cwiid_state Wiimote::getState() const {
    cwiid_state state;
    cwiid_get_state(instance,&state);
    return state;
}
