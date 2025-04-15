import cv2
import time
import threading
from collections import deque

from django.http import StreamingHttpResponse


import cv2
import threading
import time
import queue

from django.http import StreamingHttpResponse


class VideoCamera:

    def __init__(self, index: int = 0, max_queue_size=10):
        self._capture = cv2.VideoCapture(index)
        self._frame_queue = queue.Queue(maxsize=max_queue_size)
        self._running = True

        self._thread = threading.Thread(target=self._read_frames, daemon=True)
        self._thread.start()

    def __del__(self):
        self._running = False
        if self._thread.is_alive():
            self._thread.join()
        self._capture.release()

    def _read_frames(self):
        while self._running:
            success, frame = self._capture.read()
            if not success:
                continue

            fps_text = f"{int(self._capture.get(cv2.CAP_PROP_FPS))} FPS"
            cv2.putText(frame, fps_text, (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (100, 255, 0), 2)

            success, jpeg = cv2.imencode('.jpg', frame)
            if not success:
                continue

            # Очищаем очередь, если она полна
            if self._frame_queue.full():
                try:
                    self._frame_queue.get_nowait()  # удалить старый кадр
                except queue.Empty:
                    pass

            self._frame_queue.put(jpeg.tobytes())

            time.sleep(0.01)

    def get_frame(self):
        try:
            return self._frame_queue.get(timeout=1.0)
        except queue.Empty:
            return b''

    def get_generator(self):
        while True:
            frame = self.get_frame()
            if not frame:
                continue
            yield (
                b"--frame\r\n"
                b"Content-Type: image/jpeg\r\n\r\n" + frame + b"\r\n\r\n"
            )

    def get_feed(self):
        return StreamingHttpResponse(
            self.get_generator(),
            content_type="multipart/x-mixed-replace; boundary=frame"
        )

    def get_api_generator(self):
        n = 0
        while n < 10:
            print(n)
            yield b"--frame\r\nContent-Type: image/jpeg\r\n\r\n" + self.get_frame() + b"\r\n\r\n"
            n += 1

    def get_api_feed(self):
        return StreamingHttpResponse(self.get_api_generator(), content_type="multipart/x-mixed-replace; boundary=frame")
