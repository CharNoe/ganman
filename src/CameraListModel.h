#pragma once

#include <QAbstractListModel>
#include <QCameraInfo>



namespace gm {

class CameraListModel
        : public QAbstractListModel
{
public:
    CameraListModel();

    QModelIndex defaultIndex() const;
    QCameraInfo cameraInfo(const QModelIndex& index) const;
    QCameraInfo cameraInfo(const int row) const;


public: // QAbstractItemModel interface
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;


private:
    QList<QCameraInfo> _cameras;
};

} // namespace gm

