from django.shortcuts import render
from django.views.generic.base import TemplateView
from django.views.decorators.csrf import ensure_csrf_cookie
from django.apps import apps


class SessionView(TemplateView):
    template_name = 'session.html'

    def get_context_data(self, **kwargs):
        context = super().get_context_data(**kwargs)
        controller = apps.get_app_config('testbed_api').controller
        context['pins_info'] = controller.pins_info().as_dict()
        return context
