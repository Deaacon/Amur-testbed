#!/usr/bin/env python

import time
import os
import sys

import logging
import argparse
import subprocess

import celery


class RedisServer:

    def __init__(self):
        self._proc: subprocess.Popen | None = None
        self._logger = logging.getLogger('meta.redis')

    @property
    def running(self) -> bool:
        return self._proc is not None

    def run(self):
        if self.running:
            raise RuntimeError("Redis server is already running")
        self._logger.info("Running Redis server")
        self._proc = subprocess.Popen('redis-server')

    def stop(self):
        if not self.running:
            raise RuntimeError("Redis server is not running")
        self._logger.info("Stopping Redis server")
        self._proc.terminate()
        self._proc.wait(timeout=5)
        self._proc = None

    def __del__(self):
        if self.running:
            try:
                self.stop()
            except subprocess.TimeoutExpired:
                self._logger.warning("Killing Redis server after timeout expired")
                self._proc.kill()
                self._proc.wait(timeout=5)
                self._proc = None


class CeleryWorkers:

    def __init__(self, app: celery.Celery):
        self._app = app
        self._running: bool = False
        self._procs: list[subprocess.Popen] = []
        self._logger = logging.getLogger('meta.celery')

    @property
    def running(self) -> bool:
        return self._running

    def run(self):
        if self._running:
            raise RuntimeError("Celery workers are already running")
        self._logger.info("Running Celery workers")

        app_name = self._app.main
        self._procs.append(subprocess.Popen(['celery', '-A', app_name, 'worker', '-l', 'INFO']))

        self._running = True

    def _wait_stop(self):
        if self._procs:
            self._procs[0].wait(timeout=5)
        for proc in self._procs:
            proc.wait(timeout=0)
            self._procs.remove(proc)
        assert not self._procs

    def stop(self):
        if not self._running:
            raise RuntimeError("Celery workers are not running")
        self._logger.info("Stopping Celery workers")

        for proc in self._procs:
            proc.terminate()
        self._wait_stop()

        self._running = False

    def __del__(self):
        if self._running:
            try:
                self.stop()
            except subprocess.TimeoutExpired:
                self._logger.warning("Killing Celery workers after timeout expired")

                for proc in self._procs:
                    if proc.poll() is None:
                        proc.kill()
                self._wait_stop()

                self._running = False


def main():
    os.environ.setdefault('DJANGO_SETTINGS_MODULE', 'server.settings')
    try:
        from django.core.management import execute_from_command_line, call_command
        from django.core.management.commands import runserver
    except ImportError as exc:
        raise ImportError(
            "Couldn't import Django. Are you sure it's installed and "
            "available on your PYTHONPATH environment variable? Did you "
            "forget to activate a virtual environment?"
        ) from exc

    parser = argparse.ArgumentParser()
    parser.add_argument('--start-infra', action='store_true')
    parser.add_argument('--ip', type=str, default=None)
    args = parser.parse_args()

    if args.start_infra and not os.environ.get('RUN_MAIN', False):
        logging.basicConfig(level=logging.INFO)
        logger = logging.getLogger('meta')
        logger.error(f"RUNNING from {os.getpid()} {__file__} {__name__} {sys.argv} {sys.executable}")
        import pprint
        pprint.pprint(dict(os.environ))

        redis_server = RedisServer()
        redis_server.run()

        from backend import celery_app
        workers = CeleryWorkers(celery_app)
        workers.run()

    from server.wsgi import application
    assert application

    call_command('runserver', *((args.ip,) if args.ip is not None else tuple()))


if __name__ == '__main__':
    main()
