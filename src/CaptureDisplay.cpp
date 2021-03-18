#include "CaptureDisplay.h"

#include "DisplayListModel.h"


namespace gm {

CaptureDisplay::CaptureDisplay()
    : _target(-1)
    , _model(new DisplayListModel)
{

}

void CaptureDisplay::setTarget(const int index)
{
    _target = index;
}

void CaptureDisplay::capture()
{
    QPixmap frame = _model->capture(_target);
    if (frame.isNull())
        return;

    emit imageCaptured(frame.toImage());
}

QAbstractItemModel *CaptureDisplay::model()
{
    return _model.get();
}

} // namespace gm
