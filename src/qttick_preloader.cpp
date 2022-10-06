#include <qttickadaptor.h>
#include <qttick_dbus.h>
#include <qttick_event_listener.h>
#include <string>

using namespace std::string_literals;

class QtTickPreloader {
public:
    struct StartupError : public std::runtime_error { StartupError(std::string const& s) : std::runtime_error(s) {} };

    QtTickPreloader() : event_listener(), dbus(event_listener), adaptor(&dbus)
    {
        this->connectDBus();
    }

private:
    void connectDBus()
    {
        auto app_name = this->appNameFromEnvVar();
        auto bus = QDBusConnection::sessionBus();
        if (!bus.registerObject("/QtTick", &this->dbus)) {
            throw StartupError("An object is already registered. Giving up.");
        }
        if (!bus.registerService(QString::fromStdString("qt_tick."s + app_name))) {
            throw StartupError("A service with the same name is already registered.");
        }
    }

    std::string appNameFromEnvVar()
    {
        auto envvar = std::getenv("QT_TICK_SERVICE_NAME");
        if (envvar == nullptr) {
            throw StartupError{"Please set a 'QT_TICK_SERVICE_NAME' environment variable."};
        }
        return std::string{envvar};
    }

    QtTickEventListener event_listener;
    QtTickDBus dbus;
    QtTickInterfaceAdaptor adaptor;
};

static QtTickPreloader _qtTickPreloader;
