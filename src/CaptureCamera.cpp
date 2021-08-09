#include "CaptureCamera.h"

#include "CameraListModel.h"


namespace gm {

CaptureCamera::CaptureCamera()
    : _target(-1)
    , _model(new CameraListModel)
{

}

void CaptureCamera::setTarget(const int index)
{
    if (_target == index)
        return;

    QCameraInfo info = _model->cameraInfo(index);
    if (info.isNull())
        return;


    _camera.reset(new QCamera(info));
    _camera->setCaptureMode(QCamera::CaptureStillImage);

    _cameraImageCapture.reset(new QCameraImageCapture(_camera.get()));
    _cameraImageCapture->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);

    connect(_cameraImageCapture.get(), &QCameraImageCapture::imageCaptured,
            this,                      &CaptureCamera::onImageCaptured );

    QImageEncoderSettings settings;
    settings.setQuality(QMultimedia::VeryHighQuality);
    _cameraImageCapture->setEncodingSettings(settings);

    _camera->start();
}

void CaptureCamera::capture()
{
    if (_cameraImageCapture)
        _cameraImageCapture->capture();
}

QAbstractItemModel *CaptureCamera::model()
{
    return _model.get();
}

void CaptureCamera::onImageCaptured(int , const QImage &frame)
{
    QImage img = frame.copy();
    emit imageCaptured(img);
}

} // namespace gm
