#include "CameraListModel.h"

#include <algorithm>

namespace gm {

CameraListModel::CameraListModel()
    : _cameras(QCameraInfo::availableCameras())
{

}

QModelIndex CameraListModel::defaultIndex() const
{
    const QCameraInfo d = QCameraInfo::defaultCamera();
    auto result = std::find(
                _cameras.begin(), _cameras.end(),
                d );
    if (result == _cameras.end())
        return {};

    return index(std::distance(_cameras.begin(), result));
}

QCameraInfo CameraListModel::cameraInfo(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return QCameraInfo{};
    }

    return _cameras.at(index.row());
}

QCameraInfo CameraListModel::cameraInfo(const int row) const
{
    if (row < 0 || row > _cameras.size())
        return QCameraInfo{};

    return _cameras.at(row);
}

int CameraListModel::rowCount(const QModelIndex &) const
{
    return _cameras.size();
}

QVariant CameraListModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        return _cameras.at(index.row()).description();
    }

    return {};
}

} // namespace gm
