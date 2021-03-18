#pragma once

#include <QCamera>
#include <QCameraImageCapture>
#include "CaptureBase.h"



namespace gm {

class CameraListModel;


class CaptureCamera
        : public CaptureBase
{
public:
    CaptureCamera();


public: // CaptureBase interface
    virtual void
    setTarget(const int index) override;

    virtual void
    capture() override;

    virtual QAbstractItemModel*
    model() override;


private:
    void onImageCaptured(int id, const QImage& frame);

    int                                     _target;
    std::unique_ptr<CameraListModel>        _model;
    std::unique_ptr<QCamera>                _camera;
    std::unique_ptr<QCameraImageCapture>    _cameraImageCapture;
};

} // namespace gm

