import threading

import cv2

from django.http import StreamingHttpResponse


class VideoCamera:

    def __init__(self, index: int = 0):
        import os
        print(f"INIT {os.getpid()}")
        self._capture = cv2.VideoCapture(index)

    def __del__(self):
        import os
        print(f"KILL {os.getpid()}")
        self._capture.release()

    def get_frame(self):
        success, image = self._capture.read()
        assert success
        retval, jpeg = cv2.imencode('.jpg', image)
        assert retval
        return jpeg.tobytes()

    def get_generator(self):
        while True:
            yield b"--frame\r\nContent-Type: image/jpeg\r\n\r\n" + self.get_frame() + b"\r\n\r\n"

    def get_feed(self):
        return StreamingHttpResponse(self.get_generator(), content_type="multipart/x-mixed-replace; boundary=frame")
