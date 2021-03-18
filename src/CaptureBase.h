#pragma once

#include <QAbstractItemModel>


namespace gm {

class CaptureBase
        : public QObject
{
    Q_OBJECT


public:
    virtual void
    setTarget([[maybe_unused]] const int index)
    {}

    virtual void
    capture()
    {}

    virtual QAbstractItemModel*
    model()
    { return nullptr; }


signals:
    void imageCaptured  (const QImage&);
};

} // namespace gm

