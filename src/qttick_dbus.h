#ifndef _QTTICK_DBUS_H
#define _QTTICK_DBUS_H

#include <qttick_event_listener.h>
#include <QObject>
#include <QDBusVariant>

class QtTickDBus : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "qt_tick.QtTickInterface")

public:
    explicit QtTickDBus(QtTickEventListener &event_listener);

    Q_SCRIPTABLE Q_SLOT QDBusVariant invokeTickMethod(QString const& object_name, QString const& method_name);
    Q_SCRIPTABLE Q_SLOT bool invokeQtMethod(QString const& object_name, QString const& method_name);

    // TODO
//    Q_SCRIPTABLE Q_SLOT void setQtProperty(QString const& object_name, QString const& property_name, QDBusVariant const& value);

    Q_SCRIPTABLE Q_SLOT QDBusVariant getQtProperty(QString const& object_name, QString const& property_name);
    Q_SCRIPTABLE Q_SLOT bool listenToQtEvents(QString const& event_name);
    Q_SCRIPTABLE Q_SIGNAL void qtEventReceived(QString const& event_name, QString const& class_name, QString const& object_name);

private:
    void forwardQtEventReceived(QEvent::Type const& event_type, QString const& class_name, QString const& object_name);
    QEvent::Type eventFromName(QString const& name) const;
    QObject* findObjectFromName(QString const& name) const;

    QtTickEventListener &event_listener;
};

#endif // _QTTICK_DBUS_H
