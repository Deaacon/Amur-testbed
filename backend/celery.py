import os

from celery import Celery


os.environ.setdefault('DJANGO_SETTINGS_MODULE', 'server.settings')

app = Celery('backend')

app.config_from_object('django.conf:settings', namespace='CELERY')

app.autodiscover_tasks()


@app.task(bind=True, ignore_result=True)
def debug_task(self):
    print(f'Request: {self.request!r}')


class Tracker:

    def __init__(self):
        print(f"INIT CELERY TRACKER {os.getpid()}")

    def __del__(self):
        print(f"KILL CELERY TRACKER {os.getpid()}")


tracker = Tracker()
