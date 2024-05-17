from django.apps import AppConfig

from .camera import VideoCamera
from .testbed import HardwareControllerInterface, MockController, MIK32Controller


class TestbedApiConfig(AppConfig):
    default_auto_field = 'django.db.models.BigAutoField'
    name = 'testbed_api'
    SERIAL_PORT = 'COM5'

    def __init__(self, app_name, app_module):
        super().__init__(app_name, app_module)
        import os
        import sys
        print(f"TESTBED INIT {os.getpid()}, {sys.executable}, {__file__}, {__name__}")
        self.camera: VideoCamera | None = None
        self.controller: HardwareControllerInterface | None = None

    def ready(self):
        import os
        import sys
        print(f"TESTBED READY {os.getpid()}, {sys.executable}, {__file__}, {__name__}")
        if self.camera is None:
            self.camera = VideoCamera()
        if self.controller is None:
            # self.controller = MockController()
            self.controller = MIK32Controller(port=self.SERIAL_PORT)
