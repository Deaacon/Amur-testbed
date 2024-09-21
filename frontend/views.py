from django.shortcuts import render
from django.conf import settings
from django.views.generic import TemplateView
from django.views.decorators.csrf import ensure_csrf_cookie
from django.apps import apps

from testbed_api.testbed import PinsInfo


class SessionView(TemplateView):
    template_name = 'session.html'

    def get_context_data(self, **kwargs):
        context = super().get_context_data(**kwargs)
        controller = apps.get_app_config('testbed_api').controller
        pins_info: PinsInfo = controller.pins_info()
        context['pins_info'] = pins_info.as_dict()
        context['mock_camera'] = settings.TESTBED_MOCK_CAMERA
        return context
