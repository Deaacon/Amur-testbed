#!/usr/bin/env python

import os

import argparse

from django.conf import settings


def main():
    os.environ.setdefault('DJANGO_SETTINGS_MODULE', 'server.settings')
    try:
        from django.core.management import execute_from_command_line, call_command
    except ImportError as exc:
        raise ImportError(
            "Couldn't import Django. Are you sure it's installed and "
            "available on your PYTHONPATH environment variable? Did you "
            "forget to activate a virtual environment?"
        ) from exc

    parser = argparse.ArgumentParser()
    parser.add_argument('--prepare-env', action='store_true')
    parser.add_argument('--ip', type=str, default=None)
    parser.add_argument('--noreload', action='store_true')
    args = parser.parse_args()

    if not os.getenv('RUN_MAIN', False):
        print('Creating log directories')
        settings.LOG_DIR.mkdir(parents=True, exist_ok=True)
        print("Truncating old logs")
        for fp in settings.LOG_FILES:
            fp.unlink(missing_ok=True)

    from server.wsgi import application
    assert application

    server_args = []
    if args.ip is not None:
        server_args.append(args.ip)
    if args.noreload:
        server_args.append('--noreload')
    call_command('runserver', *server_args)


if __name__ == '__main__':
    main()
