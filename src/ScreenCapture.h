#pragma once

#include <QCamera>
#include <QCameraImageCapture>
#include <QImage>


namespace gm {

class ScreenCapture : public QObject
{
    Q_OBJECT

public:
    ScreenCapture(const QCameraInfo &cameraInfo);

    void capture();


signals:
    void imageCaptured(int id, const QImage& frame);


private:
    std::unique_ptr<QCamera> _camera;
    std::unique_ptr<QCameraImageCapture> _cameraImageCapture;
};

} // namespace gm

