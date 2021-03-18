#pragma once

#include "CaptureBase.h"



namespace gm {

class DisplayListModel;


class CaptureDisplay
        : public CaptureBase
{
public:
    CaptureDisplay();


public: // CaptureBase interface
    virtual void
    setTarget(const int index) override;

    virtual void
    capture() override;

    virtual QAbstractItemModel*
    model() override;


private:
    int                                 _target;
    std::unique_ptr<DisplayListModel>   _model;
};

} // namespace gm

