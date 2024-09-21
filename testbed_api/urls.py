from django.urls import path

from .views import ConfigureIOView, UploadView, IOView, MonitorView, CameraView, ApiCameraView


urlpatterns = [
    path('api/configure_io/', ConfigureIOView.as_view()),
    path('api/upload/', UploadView.as_view()),
    path('api/io/', IOView.as_view()),
    path('api/monitor/', MonitorView.as_view()),
    path('camera_feed/', CameraView.as_view()),
    path('api/camera_feed/', ApiCameraView.as_view()),
]
