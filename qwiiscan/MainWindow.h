#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <QMainWindow>
#include <QAction>
//#include <QTime>
#include <QTimer>
#include <qwt/qwt_plot_curve.h>
#include "ConnectThread.h"
#include "OscObject.h"

class MainWindow: public QMainWindow {
Q_OBJECT
public:
    MainWindow(QWidget *parent=NULL);
    ~MainWindow();
protected slots:
    void wiipoll(bool checked);
    void wiipoll_callback(void);
    void wiiconnect(bool checked);
    void wiiconnected();
signals:
    void accel(double ax,double ay,double az);
protected:
    QAction *wiiconnect_action;
    ConnectThread *wiiconnect_thread;
    QAction *wiipoll_action;
    QTimer *wiipoll_timer;

    QAction *oscforward_action;
    OscObject *osc;

    //QTime timer;
    QwtPlotCurve *accx_curve;
    QwtPlotCurve *accy_curve;
    QwtPlotCurve *accz_curve;
    double *accx;
    double *accy;
    double *accz;
    double *time;
    int datawidth;
};

#endif
