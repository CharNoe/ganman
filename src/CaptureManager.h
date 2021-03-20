#pragma once

#include <memory>
#include <QAbstractItemModel>



namespace gm {

class CaptureBase;
class ScreenCapture;
class CameraListModel;
class DisplayListModel;


class CaptureManager
        : public QObject
{
    Q_OBJECT


public: // class
    enum TargetType
    {
        None,
        Display,
        Camera,
    };

public:
    CaptureManager();
    ~CaptureManager();


    void reload         ();
    void setTargetType  (const TargetType type);
    void setTarget      (const int index);
    void capture        ();


signals:
    void modelChanged   (QAbstractItemModel*);
    void imageCaptured  (const QImage&);


private:
    CaptureBase* createCapture(const TargetType type) const;

    TargetType                      _targetType;
    std::unique_ptr<CaptureBase>    _capture;
};

} // namespace gm

