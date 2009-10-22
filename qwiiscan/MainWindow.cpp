#include "MainWindow.h"

#include <QToolBar>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), wiiconnect_thread(NULL) {

    qDebug("creating actions");
    {
        wiiconnect_action = new QAction(tr("&Connect"),this);
        wiiconnect_action->setShortcut(tr("Crtl+C"));
        //wiiconnect_action->setStatusTip(tr("Connect a wiimote"));
        connect(wiiconnect_action,SIGNAL(triggered(bool)),SLOT(wiiconnect(bool)));
    }

    qDebug("creating toolbar");
    QToolBar *toptools = new QToolBar(this);
    {
        toptools->addAction(wiiconnect_action);
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
    if (wiiconnect_thread) delete wiiconnect_thread->wiimote;
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
    } else {
        statusBar()->showMessage(tr("unable to find wiimote"));
        delete wiiconnect_thread;
        wiiconnect_thread = NULL;
        wiiconnect_action->setEnabled(true);
    }
}

