#ifndef __OSCOBJECT_H__
#define __OSCOBJECT_H__

#include <QObject>

class OscObject: public QObject {
Q_OBJECT
public:
    OscObject(QObject *parent=NULL);
public slots:
    void forward(bool f);
    void forwardButton(int n,bool state);
    void forwardAccel(double ax,double ay,double az);
protected:
    QString prefix;
    bool ff;
    void *client;
};

#endif 

