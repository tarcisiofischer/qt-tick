#include <QDebug>
#include <QMetaEnum>
#include <QMouseEvent>
#include <QCoreApplication>
#include <QApplication>
#include <QAbstractButton>
#include <QLineEdit>
#include <qttick_dbus.h>

QtTickDBus::QtTickDBus(QtTickEventListener &event_listener) : event_listener(event_listener)
{
    // Forward events to dbus
    connect(&this->event_listener, &QtTickEventListener::qtEventReceived, this, &QtTickDBus::forwardQtEventReceived);
}

// TODO: Proper error handling
QDBusVariant QtTickDBus::invokeTickMethod(QString const& object_name, QString const& method_name)
{
    auto* object = this->findObjectFromName(object_name);
    if (!object) {
        return qvariant_cast<QDBusVariant>(false);
    }

    // TODO

    return qvariant_cast<QDBusVariant>(false);
}

// TODO: Proper error handling
bool QtTickDBus::invokeQtMethod(QString const& object_name, QString const& method_name)
{
    auto* object = this->findObjectFromName(object_name);
    if (!object) {
        return false;
    }
    return QMetaObject::invokeMethod(object, method_name.toLatin1());
}

// TODO: Proper error handling
QDBusVariant QtTickDBus::getQtProperty(QString const& object_name, QString const& property_name)
{
    auto* object = this->findObjectFromName(object_name);
    if (!object) {
        return qvariant_cast<QDBusVariant>(false);
    }
    return QDBusVariant{object->property(property_name.toLatin1())};
}

bool QtTickDBus::listenToQtEvents(QString const& event_name)
{
    auto event = this->eventFromName(event_name);
    if (event == QEvent::Type::None) {
        return false;
    }
    this->event_listener.listenTo(event);
    return true;
}

QEvent::Type QtTickDBus::eventFromName(QString const& name) const
{
    auto const meta_enum = QMetaEnum::fromType<QEvent::Type>();
    auto const enum_value = meta_enum.keyToValue(name.toLatin1());
    if (enum_value == -1) {
        return QEvent::Type::None;
    }
    return QEvent::Type(enum_value);
}

void QtTickDBus::forwardQtEventReceived(QEvent::Type const& event_type, QString const& class_name, QString const& object_name)
{
    auto const meta_enum = QMetaEnum::fromType<QEvent::Type>();
    Q_EMIT this->qtEventReceived(meta_enum.valueToKey(static_cast<int>(event_type)), class_name, object_name);
}

QObject* QtTickDBus::findObjectFromName(QString const& name) const
{
    std::function<QObject*(QObject*)> findObjectFromNameRecursively = [&](QObject* obj)  -> QObject* {
        if (obj->objectName() == name) {
            return obj;
        }

        for (auto const& child : obj->children()) {
            auto* foundObj = findObjectFromNameRecursively(child);
            if (foundObj != nullptr) {
                return foundObj;
            }
        }
        return nullptr;
    };

    for (auto* obj : QApplication::topLevelWidgets()) {
        auto* foundObj = findObjectFromNameRecursively(obj);
        if (foundObj != nullptr) {
            return foundObj;
        }
    }
    return nullptr;
}
