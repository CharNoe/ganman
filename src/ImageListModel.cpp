#include "ImageListModel.h"

#include <mutex>
#include <QThread>
#include <QDir>
#include <QPixmap>
#include <QDebug>



namespace gm {

class ImageListModel::ImageLoadThread
        : public QThread
{
public:
    ImageLoadThread(const QString& path, const int maxCount);


    void                        setPath     (const QString& path);
    void                        setMaxCount (const int value);
    std::map<QString, QPixmap>  getImageMap ();


    // QThread interface
protected:
    void run() override;


private:
    std::mutex                  _mutex;
    const QString               _path;
    int                         _maxCount;
    std::map<QString, QPixmap>  _imageMap;
};

ImageListModel::ImageLoadThread::ImageLoadThread(const QString &path, const int maxCount)
    : _path(path)
    , _maxCount(maxCount)
{

}

void ImageListModel::ImageLoadThread::setMaxCount(const int value)
{
    std::lock_guard lock(_mutex);
    _maxCount = value;
}

std::map<QString, QPixmap> ImageListModel::ImageLoadThread::getImageMap()
{
    std::lock_guard lock(_mutex);
    return _imageMap;
}

void ImageListModel::ImageLoadThread::run()
{
    QDir dir(_path);
    if (!dir.exists())
    {
        return;
    }

    int maxCount;
    std::map<QString, QPixmap> imageList;
    {
        std::lock_guard lock(_mutex);
        imageList = _imageMap;
        maxCount  = _maxCount;
    }

    QStringList nameList = dir.entryList(
                QDir::Files,
                QFlags{QDir::Name, QDir::Reversed}
                );


    int                         counter = 0;
    std::map<QString, QPixmap>  newImageMap;
    for (const QString& name: nameList)
    {
        if (isInterruptionRequested())
            return;

        auto result = imageList.find(name);
        if (result != imageList.end())
        {
            newImageMap.emplace(name, std::move(result->second));
            imageList.erase(result);
        }
        else
        {
            QString path = dir.filePath(name);
            QPixmap image(path);
            if (image.isNull())
                continue;

            QPixmap scaledImage = image.scaled(
                        100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation
                        );
            if (scaledImage.isNull())
                continue;

            newImageMap.emplace(name, std::move(scaledImage));
        }

        ++counter;
        if (counter >= maxCount)
            break;
    }


    {
        std::lock_guard lock(_mutex);
        _imageMap = std::move(newImageMap);
    }
}


ImageListModel::ImageListModel(const QString& path, const int maxCount)
    : _thread(new ImageLoadThread(path, maxCount))
    , _watcher({path})
    , _oneMore(false)
    , _path(path)
    , _maxCount(maxCount)
{
    connect(_thread.get(),  &QThread::finished,
            this,           &ImageListModel::onLoadFinished );
    connect(&_watcher,      &QFileSystemWatcher::directoryChanged,
            this,           &ImageListModel::onFileChanged );

    _thread->start();
}

ImageListModel::~ImageListModel()
{
    _thread->requestInterruption();
    _thread->wait();
}

void ImageListModel::setMaxCount(const int value)
{
    if (_maxCount == value)
        return;

    _maxCount = value;
    _thread->setMaxCount(value);
    if (_thread->isRunning())
    {
        _oneMore = true;
        return;
    }

    _thread->start();
}

int ImageListModel::rowCount(const QModelIndex &) const
{
    return 2;
}

int ImageListModel::columnCount(const QModelIndex &) const
{
    return static_cast<int>(_imageMap.size());
}

QVariant ImageListModel::data(const QModelIndex &index, int role) const
{
    const int row = index.row();
    const int col = index.column();
    if (row < 0 || col < 0)
        return {};


    if (static_cast<unsigned>(col) > _imageMap.size())
        std::exit(EXIT_FAILURE);


    auto it = _imageMap.begin();
    std::advance(it, col);

    if (role == UserRole_Path)
    {
        return _path + '/' + it->first;
    }

    if (row == 0) // image
    {
        if (role == Qt::DecorationRole)
        {
            return it->second;
        }
        else if (role == Qt::SizeHintRole)
        {
            return QSize(it->second.width() + 6, 0);
        }
    }
    else if (row == 1) // name
    {
        if (role == Qt::DisplayRole)
        {
            return it->first;
        }
        else if (role == Qt::SizeHintRole)
        {
            return QSize(it->second.width() + 6, 0);
        }
    }


    return {};
}

void ImageListModel::onLoadFinished()
{
    if (_oneMore)
    {
        _oneMore = false;
        _thread->start();
        return;
    }

    if (_imageMap.size())
    {
        auto size = _imageMap.size();
        beginRemoveColumns({}, 0, size-1);

        _imageMap.clear();

        endRemoveColumns();
    }


    auto imageMap = _thread->getImageMap();
    if (imageMap.size() == 0)
        return;

    beginInsertColumns({}, 0, imageMap.size()-1);

    _imageMap = std::move(imageMap);

    endInsertColumns();
}

void ImageListModel::onFileChanged(const QString &)
{
    if (_thread->isRunning())
    {
        _oneMore = true;
        return;
    }

    _thread->start();
}

} // namespace gm
