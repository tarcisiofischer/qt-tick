#ifndef _QTTICK_EVENT_LISTENER_H
#define _QTTICK_EVENT_LISTENER_H

#include <QObject>
#include <QEvent>
#include <vector>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

class QtTickEventListener : public QObject {
    Q_OBJECT

public:
    struct ListenerError : public std::runtime_error { ListenerError(std::string const& s) : std::runtime_error(s) {} };

    QtTickEventListener();
    ~QtTickEventListener();

    void listenTo(QEvent::Type event);
    bool eventFilter(QObject *object, QEvent *event) override;

    Q_SIGNAL void qtEventReceived(QEvent::Type event, QString class_name, QString object_name);

private:
    static auto constexpr APP_STARTUP_WAIT_TIME = 250ms;
    static auto constexpr APP_STARTUP_MAX_WAIT_TIME = 1s;

    static void startupMonitor(QtTickEventListener *self);

    std::thread monitor;
    std::vector<QEvent::Type> listening_to;
};

#endif // _QTTICK_EVENT_LISTENER_H
