import time

import enum
from dataclasses import dataclass, asdict

import threading

import serial

from .mik32 import upload


def asdict_factory(data):

    def convert_value(obj):
        if isinstance(obj, enum.Enum):
            return obj.value
        return obj

    return dict((k, convert_value(v)) for k, v in data)


class IOMode(enum.Enum):
    NOTSET = None
    INPUT = 'input'
    OUTPUT = 'output'


class IOState(enum.Enum):
    UNDEFINED = None
    LOW = 0
    HIGH = 1


@dataclass
class Pin:
    index: int
    name: str
    mode: IOMode = IOMode.NOTSET
    state: IOState = IOState.UNDEFINED


@dataclass
class PinsInfo:
    pins: list[Pin]

    def as_dict(self):
        return asdict(self, dict_factory=asdict_factory)


class IOMonitorInterface:

    def start(self):
        ...

    def stop(self):
        ...

    def get_period(self) -> float:
        ...

    def get_start_ts(self) -> float:
        ...

    def get_data(self) -> list[tuple[float, tuple[IOState]]]:
        ...

    def get_current_data(self) -> tuple[float, tuple[IOState]]:
        ...

    def set_data(self, data: tuple[IOState]):
        ...


class MockMonitor(IOMonitorInterface):

    def __init__(self, pins_info: PinsInfo):
        self._pins_info = pins_info
        self._thread = None
        self._stop_event = threading.Event()
        self._lock = threading.RLock()
        self._start_ts = None
        self._period = 0.1
        self._data = []

    def start(self):
        if self._thread is not None:
            raise RuntimeError("Monitor is already started")

        self._data.clear()
        self._stop_event.clear()
        self._thread = threading.Thread(target=self._do_monitor, daemon=True)

        self._start_ts = time.time()
        self._thread.start()

    def stop(self):
        if self._thread is None:
            raise RuntimeError("Monitor is not started")

        self._stop_event.set()
        self._thread.join()

        self._start_ts = None
        self._thread = None

    def _do_monitor(self):
        while not self._stop_event.is_set():
            with self._lock:
                ts = time.time()
                state_index = int((ts - self._start_ts) // self._period)
                states = tuple(
                    IOState.HIGH if ((state_index >> e) & 0b1) else IOState.LOW
                    for e in range(len(self._pins_info.pins))
                )
                self._data.append((ts, states))
            time.sleep(self._period)

    def get_data(self) -> list[tuple[float, tuple[IOState]]]:
        with self._lock:
            return self._data[:]

    def get_current_data(self) -> tuple[float, tuple[IOState]]:
        with self._lock:
            if self._data:
                return self._data[-1]
            else:
                return time.time(), tuple(IOState.UNDEFINED for _ in range(len(self._pins_info.pins)))

    def __del__(self):
        self._stop_event.set()
        self._thread.join()


class HardwareControllerInterface:

    def clean(self):
        ...

    def program(self, software: bytes):
        ...

    def pins_info(self) -> PinsInfo:
        ...

    def configure_inputs(self, modes: list[IOMode]):
        ...

    def get_monitor(self) -> IOMonitorInterface:
        ...


class MockController(HardwareControllerInterface):

    def __init__(self,):
        self._pins_info = PinsInfo(pins=[])
        for i in range(0, 10):
            self._pins_info.pins.append(Pin(index=i, name=f'PORT_{i}', mode=IOMode.INPUT))
        for i in range(10, 20):
            self._pins_info.pins.append(Pin(index=i, name=f'PORT_{i}', mode=IOMode.OUTPUT))
        self._monitor = MockMonitor(self._pins_info)

    def clean(self):
        for pin in self._pins_info.pins:
            pin.mode = IOMode.NOTSET
            pin.state = IOState.UNDEFINED

    def program(self, software: bytes):
        upload(software)

    def pins_info(self) -> PinsInfo:
        return self._pins_info

    def configure_inputs(self, modes: list[IOMode]):
        if len(modes) != len(self._pins_info.pins):
            raise IndexError(f"List of length {len(self._pins_info.pins)} expected, got {len(modes)}")
        for pin, mode in zip(self._pins_info.pins, modes):
            assert isinstance(mode, IOMode)
            pin.mode = mode

    def get_monitor(self) -> MockMonitor:
        return self._monitor


class MIK32Monitor(IOMonitorInterface):

    def __init__(self, pins_info: PinsInfo, port: str):
        self._pins_info = pins_info
        self._port = port
        self._serial: None | serial.Serial = None
        self._thread = None
        self._stop_event = threading.Event()
        self._lock = threading.RLock()
        self._start_ts = None
        self._period = 0.1
        self._data = []

    def start(self):
        if self._thread is not None:
            raise RuntimeError("Monitor is already started")

        self._data.clear()
        self._stop_event.clear()
        self._thread = threading.Thread(target=self._do_monitor, daemon=True)
        self._serial = serial.Serial(port=self._port)

        self._start_ts = time.time()
        self._thread.start()

    def stop(self):
        if self._thread is None:
            raise RuntimeError("Monitor is not started")

        self._stop_event.set()
        self._thread.join()
        self._serial.close()

        self._start_ts = None
        self._thread = None
        self._serial = None

    def _do_monitor(self):
        while not self._stop_event.is_set():
            with self._lock:
                ts = time.time()
                states = tuple(pin.state for pin in self._pins_info.pins)
                self._data.append((ts, states))
            time.sleep(self._period)

    def get_data(self) -> list[tuple[float, tuple[IOState]]]:
        with self._lock:
            return self._data[:]

    def get_current_data(self) -> tuple[float, tuple[IOState]]:
        with self._lock:
            if self._data:
                return self._data[-1]
            else:
                return time.time(), tuple(pin.state for pin in self._pins_info.pins)

    def set_data(self, data: tuple[IOState]):
        n_data = len(data)
        inputs = tuple(pin for pin in self._pins_info.pins if pin.mode == IOMode.INPUT)
        n_inputs = len(inputs)
        if n_data != n_inputs:
            raise IndexError(f"Invalid input data length {n_data}, expected {n_inputs}")
        data_int = 0b0
        with self._lock:
            for i, (pin, state) in enumerate(zip(inputs, data)):
                data_int += (0b1 if state == IOState.HIGH else 0b0) << i
                pin.state = state
            n_bytes = max((data_int.bit_length() + 7) // 8, 1)
            assert n_bytes == 1
            data_bytes = data_int.to_bytes(n_bytes, byteorder='big')  # TODO Determine order
            self._serial.write(data_bytes)

    def __del__(self):
        self._stop_event.set()
        self._thread.join()
        self._serial.close()


class MIK32Controller(HardwareControllerInterface):

    def __init__(self, port: str):
        self._pins_info = PinsInfo(pins=[])
        self._pins_info.pins.append(Pin(index=0, name='PORT_0_0', mode=IOMode.INPUT))
        self._monitor = MIK32Monitor(self._pins_info, port)

    def clean(self):
        for pin in self._pins_info.pins:
            pin.state = IOState.UNDEFINED

    def program(self, software: bytes):
        upload(software)

    def pins_info(self) -> PinsInfo:
        return self._pins_info

    def configure_inputs(self, modes: list[IOMode]):
        raise NotImplementedError("MIK32 testbed pins are not configurable")

    def get_monitor(self) -> MIK32Monitor:
        return self._monitor
