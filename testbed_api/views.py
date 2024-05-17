import json
import http

from django.http import HttpRequest, HttpResponse, JsonResponse
from django.shortcuts import render
from django.views.generic.base import View
from django.apps import apps
from django.conf.urls.static import static

from .mik32 import TestbedError, UploadError
from .testbed import IOState


class TestbedAccessorMixin:

    @property
    def app_config(self):
        return apps.get_app_config('testbed_api')

    @property
    def controller(self):
        return self.app_config.controller


class ConfigureIOView(View, TestbedAccessorMixin):

    def post(self, request: HttpRequest, *args, **kwargs):
        print(request)


class IOView(View, TestbedAccessorMixin):

    def get(self, request: HttpRequest, *args, **kwargs):
        # print("Get")
        data = self.controller.get_monitor().get_data()
        # print("Got data", len(data))
        return JsonResponse({'time': [e[0] for e in data], 'states': [[pin.value for pin in e[1]] for e in data]})

    def post(self, request: HttpRequest, *args, **kwargs):
        json_data = json.loads(request.body)
        data = [IOState.HIGH if e is True else (IOState.LOW if e is False else IOState.UNDEFINED) for e in json_data]
        monitor = self.controller.get_monitor()
        monitor.set_data(data)
        return JsonResponse({})


class MonitorView(View, TestbedAccessorMixin):

    def post(self, request: HttpRequest):
        data = request.body
        match data:
            case b'start':
                self.controller.get_monitor().start()
                return JsonResponse({})
            case b'stop':
                self.controller.get_monitor().stop()
                return JsonResponse({})
            case _:
                return JsonResponse(
                    {'error': "Unknown command \"{}\"".format(data)},
                    status=http.HTTPStatus.BAD_REQUEST
                )


class CameraView(View, TestbedAccessorMixin):

    def get(self, request: HttpRequest, *args, **kwargs):
        return self.app_config.camera.get_feed()


class MockCameraView(View):

    def get(self, request: HttpRequest, *args, **kwargs):
        return static('MIK32Static.jpg')


class UploadView(View, TestbedAccessorMixin):

    def post(self, request: HttpRequest, *args, **kwargs):
        print("UPLOAD", len(request.body))
        try:
            self.app_config.controller.program(request.body)
        except UploadError as error:
            return JsonResponse({'error': str(error)}, status=http.HTTPStatus.INTERNAL_SERVER_ERROR)
        return JsonResponse({})
