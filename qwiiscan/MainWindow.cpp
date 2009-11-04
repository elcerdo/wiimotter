#include "MainWindow.h"

#include <QToolBar>
#include <QStatusBar>
#include <qwt/qwt_plot.h>

#define DATALEN 512

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), wiiconnect_thread(NULL) {

    qDebug("creating timer and plot");
    {
        wiipoll_timer = new QTimer(this);
        wiipoll_timer->setInterval(5);
        connect(wiipoll_timer,SIGNAL(timeout(void)),SLOT(wiipoll_callback(void)));

        QwtPlot *plot = new QwtPlot(this);
        plot->setAxisScale(QwtPlot::xBottom,0,(DATALEN-1)*.05);
        plot->setAxisScale(QwtPlot::yLeft,-127,127);
        this->setCentralWidget(plot);
        
        accx = new double[DATALEN];
        accy = new double[DATALEN];
        accz = new double[DATALEN];
        time = new double[DATALEN];
        for (int k=0; k<DATALEN; k++) time[k] = k*.05;

        accx_curve = new QwtPlotCurve("accx");
        accy_curve = new QwtPlotCurve("accx");
        accz_curve = new QwtPlotCurve("accx");
        accx_curve->setPen(QPen(QColor(qRgb(255,0,0))));
        accy_curve->setPen(QPen(QColor(qRgb(0,255,0))));
        accz_curve->setPen(QPen(QColor(qRgb(0,0,255))));
        accx_curve->attach(plot);
        accy_curve->attach(plot);
        accz_curve->attach(plot);
    }

    qDebug("creating osc client");
    {
        osc = new OscObject(this);
        connect(this,SIGNAL(accel(double,double,double)),osc,SLOT(forwardAccel(double,double,double)));
    }

    qDebug("creating actions");
    {
        wiiconnect_action = new QAction(tr("&Connect"),this);
        wiiconnect_action->setShortcut(tr("Ctrl+C"));
        //wiiconnect_action->setStatusTip(tr("Connect a wiimote"));
        connect(wiiconnect_action,SIGNAL(triggered(bool)),SLOT(wiiconnect(bool)));

        wiipoll_action = new QAction(tr("&Poll"),this);
        wiipoll_action->setShortcut(tr("Ctrl+P"));
        wiipoll_action->setCheckable(true);
        connect(wiipoll_action,SIGNAL(triggered(bool)),SLOT(wiipoll(bool)));

        oscforward_action = new QAction(tr("&Forward"),this);
        oscforward_action->setShortcut(tr("Ctrl+F"));
        oscforward_action->setCheckable(true);
        connect(oscforward_action,SIGNAL(triggered(bool)),osc,SLOT(forward(bool)));
    }

    qDebug("creating toolbar");
    QToolBar *toptools = new QToolBar(this);
    {
        toptools->addAction(wiiconnect_action);
        toptools->addAction(wiipoll_action);
        toptools->addAction(oscforward_action);
        this->addToolBar(Qt::TopToolBarArea,toptools);
    }

    qDebug("creating status bar");
    {
        QStatusBar *statusbar = new QStatusBar(this);
        this->setStatusBar(statusbar);
    }

    this->setMinimumSize(400,400);
}

MainWindow::~MainWindow() {
    if ( wiiconnect_thread && wiiconnect_thread->wiimote ) delete wiiconnect_thread->wiimote;
    delete accx_curve;
    delete accy_curve;
    delete accz_curve;
    delete [] accx;
    delete [] accy;
    delete [] accz;
}

void MainWindow::wiipoll(bool start) {
    if (start) {
        wiipoll_timer->start();
        //timer.restart();
        datawidth = 0;
    } else wiipoll_timer->stop();
}

void MainWindow::wiipoll_callback() {
    if ( wiiconnect_thread == NULL || wiiconnect_thread->wiimote == NULL ) return;

    for (int k=0; k<DATALEN-1; k++) {
        accx[k] = accx[k+1];
        accy[k] = accy[k+1];
        accz[k] = accz[k+1];
        //time[k] = time[k+1];
    }

    cwiid_state state = wiiconnect_thread->wiimote->getState();
    accx[DATALEN-1] = 128-state.acc[0];
    accy[DATALEN-1] = 128-state.acc[1];
    accz[DATALEN-1] = 128-state.acc[2];
    emit accel(accx[DATALEN-1]/128.,accy[DATALEN-1]/128.,accz[DATALEN-1]/128.);
    //time[DATALEN-1] = timer.elapsed()*1e-3;
    if (datawidth<(DATALEN-1)) datawidth++;

    //qDebug("acc %d %f %f %f %f",datawidth,accx[DATALEN-1],accy[DATALEN-1],accz[DATALEN-1],time[DATALEN-1]);

    QwtPlot *plot = static_cast<QwtPlot*>(this->centralWidget());
    accx_curve->setData(&time[DATALEN-datawidth],&accx[DATALEN-datawidth],datawidth);
    accy_curve->setData(&time[DATALEN-datawidth],&accy[DATALEN-datawidth],datawidth);
    accz_curve->setData(&time[DATALEN-datawidth],&accz[DATALEN-datawidth],datawidth);
    //plot->setAxisAutoScale(QwtPlot::yLeft);
    //plot->setAxisAutoScale(QwtPlot::xBottom);
    plot->replot();

}

void MainWindow::wiiconnect(bool checked) {
    Q_UNUSED(checked);
    Q_ASSERT(wiiconnect_thread == NULL and checked == false);

    statusBar()->showMessage("scanning for wiimote");
    wiiconnect_action->setEnabled(false);
    wiiconnect_thread = new ConnectThread;
    wiiconnect_thread->start();
    connect(wiiconnect_thread,SIGNAL(finished(void)),SLOT(wiiconnected(void)));
}

void MainWindow::wiiconnected(void) {
    Q_ASSERT(wiiconnect_thread != NULL && wiiconnect_thread->isFinished());

    if (wiiconnect_thread->wiimote != NULL) {
        statusBar()->showMessage(tr("found wiimote %1 !!!").arg(wiiconnect_thread->wiimote->getAddress()));
        connect(wiiconnect_thread->wiimote,SIGNAL(buttonToggled(int,bool)),osc,SLOT(forwardButton(int,bool)));
    } else {
        statusBar()->showMessage(tr("unable to find wiimote"));
        delete wiiconnect_thread;
        wiiconnect_thread = NULL;
        wiiconnect_action->setEnabled(true);
    }
}

