import dbus
import subprocess
import os
import pytest
import time

@pytest.fixture
def runapp():
    assert 'TICK_PATH' in os.environ, 'You must set "TICK_PATH" env var to the libqt_tick dynamic library folder'
    assert 'APP_PATH' in os.environ, 'You must set "APP_PATH" env var to the example-calculator executable folder'

    tick_path = os.environ.get('TICK_PATH')
    app_path = os.environ.get('APP_PATH')
    os.environ['QT_TICK_SERVICE_NAME'] = 'AppTesting'
    os.environ['LD_PRELOAD'] = f'{tick_path}/libqt_tick.so'
    cmd = f'{app_path}/example-calculator'
    app = subprocess.Popen([cmd], stdout=subprocess.PIPE, env=os.environ)

    # Wait startup
    time.sleep(1)

    yield
    app.kill()


def test_application(runapp):
    bus = dbus.SessionBus()
    proxy = bus.get_object('qt_tick.AppTesting', '/QtTick')
    proxy.invokeQtMethod('btn_4', 'click')
    proxy.invokeQtMethod('btn_0', 'click')
    proxy.invokeQtMethod('btn_+', 'click')
    proxy.invokeQtMethod('btn_2', 'click')
    proxy.invokeQtMethod('btn_=', 'click')
    assert proxy.getQtProperty('output', 'text') == '42'
