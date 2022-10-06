#include <qttick_event_listener.h>
#include <QApplication>
#include <QDebug>

QtTickEventListener::QtTickEventListener() : monitor(&QtTickEventListener::startupMonitor, this)
{
}

QtTickEventListener::~QtTickEventListener()
{
    this->monitor.join();
}

void QtTickEventListener::listenTo(QEvent::Type event)
{
    this->listening_to.push_back(event);
}

bool QtTickEventListener::eventFilter(QObject *object, QEvent *event)
{
    auto s = std::find(this->listening_to.cbegin(), this->listening_to.cend(), event->type());
    if (s == this->listening_to.cend()) {
        return false;
    }

    Q_EMIT this->qtEventReceived(event->type(), object->metaObject()->className(), object->objectName());
    return false;
}

void QtTickEventListener::startupMonitor(QtTickEventListener *self)
{
    auto cumtime = 0ms;
    while (QApplication::instance() == nullptr) {
        std::this_thread::sleep_for(APP_STARTUP_WAIT_TIME);
        cumtime += APP_STARTUP_WAIT_TIME;
        if (cumtime > APP_STARTUP_MAX_WAIT_TIME) {
            throw ListenerError{"Could not connect to application (timeout)"};
        }
    }
    QApplication::instance()->installEventFilter(self);
}