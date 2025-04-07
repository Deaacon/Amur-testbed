import cv2
import time
import threading
from collections import deque

from django.http import StreamingHttpResponse


class VideoCamera:

    def __init__(self, index: int = 0):
        self._capture = cv2.VideoCapture(index)
        self._capture.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc('X', 'V', 'I', 'D'))
        self._prev_frame_time = 0
        self._new_frame_time = 0
        
        self._frames = deque()
        self._thread = threading.Thread(target = self.read_frames)
        self._thread.start()

    
    def __del__(self):
        self._capture.release()
        self._thread.join()

    
    def get_frame(self):
        while (not self._frames):
            time.sleep(0.005)
        return self._frames.pop()


    def read_frames(self):
        while True:
            self._new_frame_time = time.time()
            _, frame = self._capture.read()

            fps = 1 / (self._new_frame_time - self._prev_frame_time)
            self._prev_frame_time = self._new_frame_time

            fps = int(fps)
            fps = str(fps)
            cv2.putText(frame, fps, (7, 70), cv2.FONT_HERSHEY_SIMPLEX, 3, (100, 255, 0), 3, cv2.LINE_AA)

            _, frame = cv2.imencode('.jpg', frame)
            self._frames.append(frame.tobytes())

    
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
