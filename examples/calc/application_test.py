import dbus
import subprocess
import os
import pytest
import time

@pytest.fixture
def runapp():
    os.environ['QT_TICK_SERVICE_NAME'] = 'AppTesting'

    # TODO: Properly search for binaries (paths missing)
    os.environ['LD_PRELOAD'] = 'libqt_tick.so'
    cmd = "example-calculator"

    app = subprocess.Popen([cmd], stdout=subprocess.PIPE, env=os.environ)

    # Wait startup
    time.sleep(1)

    yield
    app.kill()


def test_application(runapp):
    bus = dbus.SessionBus()
    proxy = bus.get_object('qt_tick.AppTesting', "/QtTick")
    proxy.invokeQtMethod('btn_4', 'click')
    proxy.invokeQtMethod('btn_0', 'click')
    proxy.invokeQtMethod('btn_+', 'click')
    proxy.invokeQtMethod('btn_2', 'click')
    proxy.invokeQtMethod('btn_=', 'click')
    assert proxy.invokeTickMethod('output', 'QLineEdit::text') == "42"
