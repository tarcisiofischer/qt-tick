#ifndef _QTTICK_DBUS_H
#define _QTTICK_DBUS_H

#include <qttick_event_listener.h>
#include <QObject>

class QtTickDBus : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "qt_tick.QtTickInterface")

public:
    explicit QtTickDBus(QtTickEventListener &event_listener);

    Q_SCRIPTABLE Q_SLOT bool listenToQtEvents(QString const& event_name);
    Q_SCRIPTABLE Q_SIGNAL void qtEventReceived(QString const& event_name, QString const& class_name, QString const& object_name);

private:
    void forwardQtEventReceived(QEvent::Type const& event, QString const& class_name, QString const& object_name);

    QtTickEventListener &event_listener;
};

#endif // _QTTICK_DBUS_H
