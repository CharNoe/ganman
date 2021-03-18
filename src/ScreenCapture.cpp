#include "ScreenCapture.h"

#include <QDebug>
#include <QCameraInfo>


namespace gm {

ScreenCapture::ScreenCapture(const QCameraInfo &cameraInfo)
{
    _camera.reset(new QCamera(cameraInfo));
    _camera->setCaptureMode(QCamera::CaptureStillImage);

    _cameraImageCapture.reset(new QCameraImageCapture(_camera.get()));
    _cameraImageCapture->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);
    connect(_cameraImageCapture.get(), &QCameraImageCapture::imageCaptured,
            this,                      &ScreenCapture::imageCaptured );

    QImageEncoderSettings settings;
    settings.setQuality(QMultimedia::VeryHighQuality);
    _cameraImageCapture->setEncodingSettings(settings);

    _camera->start();
}

void ScreenCapture::capture()
{
    _cameraImageCapture->capture();
}

} // namespace gm
