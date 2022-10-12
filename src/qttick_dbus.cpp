#include <QDebug>
#include <QMetaEnum>
#include <QMouseEvent>
#include <QCoreApplication>
#include <QApplication>
#include <QAbstractButton>
#include <QLineEdit>
#include <QMenu>
#include <QDBusArgument>
#include <qttick_dbus.h>

QtTickDBus::QtTickDBus(QtTickEventListener &event_listener) : event_listener(event_listener)
{
    // Forward events to dbus
    connect(&this->event_listener, &QtTickEventListener::qtEventReceived, this, &QtTickDBus::forwardQtEventReceived);
}

QDBusVariant QtTickDBus::invokePluginMethod(QString const& plugin_name, QString const& method_name)
{
    // TODO
    return qvariant_cast<QDBusVariant>(false);
}

// TODO: Should it be the QEVent itself instead of a QVariantMap? If yes, how to make a clone of an event in qt<6?
void QtTickDBus::triggerQtEvent(QString const& object_name, QVariantMap const& event_metadata)
{
    auto* object = this->findObjectFromName(object_name);
    auto event_type_str = event_metadata["event_type"].toString();
    if (event_type_str == "MouseButtonPress" || event_type_str == "MouseButtonRelease") {
        this->triggerQtMouseEvent(object, event_metadata);
    } else {
        throw QTTickDbusError("No handler to qt event named \"" + event_type_str.toStdString() + "\"");
    }
}

void QtTickDBus::invokeQtMethod(QString const& object_name, QString const& method_name)
{
    auto* object = this->findObjectFromName(object_name);
    if (!QMetaObject::invokeMethod(object, method_name.toLatin1())) {
        throw QTTickDbusError("Could not invoke method " + method_name.toStdString() + " in object " + object_name.toStdString());
    }
}

QDBusVariant QtTickDBus::getQtProperty(QString const& object_name, QString const& property_name)
{
    auto* object = this->findObjectFromName(object_name);
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
    throw QTTickDbusError("Couldn't find object named " + name.toStdString());
    return nullptr;
}

void QtTickDBus::triggerQtMouseEvent(QObject* object, QVariantMap const& event_metadata)
{
    auto event_type_str = event_metadata["event_type"].toString();
    auto event_type = [&]() {
        if (event_type_str == "MouseButtonPress") {
            return QEvent::MouseButtonPress;
        } else if (event_type_str == "MouseButtonRelease") {
            return QEvent::MouseButtonRelease;
        }
        throw QTTickDbusError("Unknown event type " + event_type_str.toStdString());
    }();
    auto local_pos = QPointF{event_metadata["local_pos_x"].toDouble(), event_metadata["local_pos_y"].toDouble()};
    auto mouse_button = [&]() {
        auto btnString = event_metadata["mouse_button"].toString();
        if (btnString == "LeftButton") {
            return Qt::MouseButton::LeftButton;
        } else if (btnString == "RightButton") {
            return Qt::MouseButton::RightButton;
        } else if (btnString == "MiddleButton") {
            return Qt::MouseButton::MiddleButton;
        }
        throw QTTickDbusError("Unknown button type " + btnString.toStdString());
    }();
    auto modifiers = Qt::KeyboardModifier::NoModifier;
    auto event = new QMouseEvent(event_type, local_pos, mouse_button, mouse_button, modifiers);
    QApplication::postEvent(object, event);
}