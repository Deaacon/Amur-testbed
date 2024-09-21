import os

import logging
import subprocess

logger = logging.getLogger('testbed')

FILE_PATH = os.path.dirname(__file__)
PIO_PROJECT_PATH = os.path.join(FILE_PATH, 'platformio_project')
SOFTWARE_SOURCE_PATH = os.path.join(PIO_PROJECT_PATH, "src/software.c")
SOFTWARE_BINARY_PATH = os.path.join(PIO_PROJECT_PATH, ".pio/build/mik32v0/firmware.hex")


class TestbedError(Exception):
    pass


class UploadError(TestbedError):
    pass


def upload(software: bytes) -> str:
    with open(SOFTWARE_SOURCE_PATH, mode='wb') as f:
        f.write(software)
    proc = subprocess.Popen(
        ['pio', 'run', '-e', 'mik32v2', '--target', 'upload', '--verbose'],
        # [PIO_EXECUTABLE_PATH, '--help'],
        stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
        # stdin=sys.stdin, stdout=sys.stdout, stderr=sys.stderr,
        cwd=PIO_PROJECT_PATH,
    )
    out, err = proc.communicate(timeout=20)
    output = (out + err).decode(errors='ignore')
    if proc.returncode != 0:
        logger.error(f"Upload error:\n{output}")
        raise UploadError(output)
    return output
