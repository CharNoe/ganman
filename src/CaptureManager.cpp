#include "CaptureManager.h"

#include "CaptureDisplay.h"
#include "CaptureCamera.h"
#include "CameraListModel.h"
#include "DisplayListModel.h"



namespace gm {

CaptureManager::CaptureManager()
    : _targetType(None)
    , _capture(new CaptureBase)
{
}


void CaptureManager::reload()
{
    _capture.reset(createCapture(_targetType));
    connect(_capture.get(), &CaptureBase::imageCaptured,
            this, &CaptureManager::imageCaptured );

    emit modelChanged(_capture->model());
}

void CaptureManager::setTargetType(const CaptureManager::TargetType type)
{
    if (_targetType == type)
        return;

    _targetType = type;
    reload();
}

void CaptureManager::setTarget(const int index)
{
    _capture->setTarget(index);
}

void CaptureManager::capture()
{
    _capture->capture();
}

CaptureBase *CaptureManager::createCapture(const TargetType type) const
{
    if (type == None)
    {
        return new CaptureBase;
    }
    else if (type == Display)
    {
        return new CaptureDisplay;
    }
    else if (type == Camera)
    {
        return new CaptureCamera;
    }
    else
    {
        std::exit(EXIT_FAILURE);
    }
}

} // namespace gm
