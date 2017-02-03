#include "networkthreader.h"
#include "mainwindow.h"
#include <QMetaObject>

void tryit(void* object_ptr) {
    QMetaObject qtmo;
    qtmo.invokeMethod((MainWindow*)object_ptr, "messageHandler_slot", Q_ARG(QString, QString("testing")));
}
