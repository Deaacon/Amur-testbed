from django.apps import AppConfig
from django.conf import settings

from .camera import VideoCamera
from .testbed import HardwareControllerInterface, MockController, MIK32Controller


class TestbedApiConfig(AppConfig):
    default_auto_field = 'django.db.models.BigAutoField'
    name = 'testbed_api'

    def __init__(self, app_name, app_module):
        super().__init__(app_name, app_module)
        self.camera: VideoCamera | None = None
        self.controller: HardwareControllerInterface | None = None

    def ready(self):
        if self.camera is None:
            self.camera = VideoCamera(index=settings.TESTBED_CAMERA_INDEX)
        if self.controller is None:
            if settings.TESTBED_MOCK_SIGNALS:
                self.controller = MockController()
            else:
                self.controller = MIK32Controller(port=settings.TESTBED_SERIAL_PORT)
