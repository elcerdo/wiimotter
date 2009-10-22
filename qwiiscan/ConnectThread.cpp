#include "ConnectThread.h"

#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

ConnectThread::ConnectThread(QObject *parent) : QThread(parent), wiimote(NULL) {}

void ConnectThread::run() {
    Q_ASSERT(this->wiimote == NULL);
    qDebug("connection thread hello");

    inquiry_info *ii = NULL;
    inquiry_info *valid = NULL;
    int max_rsp, num_rsp;
    int dev_id, sock, len, flags;
    int i;
    char addr[19];
    char name[248];

	qDebug("looking for bluetooth adapter");
    dev_id = hci_get_route(NULL);
	qDebug("opening socket to bluetooth adapter");
    sock = hci_open_dev( dev_id );
    if (dev_id < 0 || sock < 0) {
        perror("opening socket");
        exit(1);
    }

    len  = 3;
    max_rsp = 255;
    flags = IREQ_CACHE_FLUSH;
    ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));
    
	qDebug("scanning for devices for %.1fs",1.28*len);
    num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
    if( num_rsp < 0 ) perror("hci_inquiry");

	qDebug("found %d device(s)",num_rsp);
    for (i = 0; i < num_rsp; i++) {
        ba2str(&(&ii[i])->bdaddr, addr);
        memset(name, 0, sizeof(name));
        if (hci_read_remote_name(sock, &ii[i].bdaddr, sizeof(name), name, 0) < 0) strcpy(name, "unknown");
        if (valid == NULL and strncmp(name,"Nintendo",8) == 0) valid = &ii[i];
        qDebug("** %s  [%s]", addr, name);
    }

    if (valid != NULL) {
        qDebug("found wiimote");
        this->wiimote = new Wiimote(valid->bdaddr);
    }

    free( ii );
    close( sock );

    qDebug("connection thread bye");
}

