import cv2

from django.http import StreamingHttpResponse


class VideoCamera:

    def __init__(self, index: int = 0):
        self._capture = cv2.VideoCapture(index)

    def __del__(self):
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

    def get_api_generator(self):
        n = 0
        while n < 10:
            print(n)
            yield b"--frame\r\nContent-Type: image/jpeg\r\n\r\n" + self.get_frame() + b"\r\n\r\n"
            n += 1

    def get_feed(self):
        return StreamingHttpResponse(self.get_generator(), content_type="multipart/x-mixed-replace; boundary=frame")

    def get_api_feed(self):
        return StreamingHttpResponse(self.get_api_generator(), content_type="multipart/x-mixed-replace; boundary=frame")
