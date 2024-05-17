import os


class Tracker:

    def __init__(self):
        print(f"INIT TRACKER {os.getpid()}")

    def __del__(self):
        print(f"KILL TRACKER {os.getpid()}")


tracker = Tracker()
