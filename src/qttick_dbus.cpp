#include <QDebug>
#include <QMetaEnum>
#include <qttick_dbus.h>

QtTickDBus::QtTickDBus(QtTickEventListener &event_listener) : event_listener(event_listener)
{
    // Forward events to dbus
    connect(&this->event_listener, &QtTickEventListener::qtEventReceived, this, &QtTickDBus::forwardQtEventReceived);
}

bool QtTickDBus::listenToQtEvents(QString const& event_name)
{
    auto const& std_string = event_name.toStdString();
    auto meta_enum = QMetaEnum::fromType<QEvent::Type>();
    auto enum_value = meta_enum.keyToValue(std_string.c_str());
    if (enum_value == -1) {
        return false;
    }

    this->event_listener.listenTo(QEvent::Type(enum_value));
    return true;
}

void QtTickDBus::forwardQtEventReceived(QEvent::Type const& event, QString const& class_name, QString const& object_name)
{
    auto meta_enum = QMetaEnum::fromType<QEvent::Type>();
    Q_EMIT this->qtEventReceived(meta_enum.valueToKey(static_cast<int>(event)), class_name, object_name);
}
