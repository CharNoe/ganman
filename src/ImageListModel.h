#pragma once

#include <QAbstractListModel>
#include <QFileSystemWatcher>



namespace gm {

class ImageListModel
        : public QAbstractListModel
{
public: // classes
    enum UserRole
    {
        UserRole_Path = Qt::UserRole + 1
    };


public:
    ImageListModel(const QString& path, const int maxCount);
    ~ImageListModel();

    void setMaxCount(const int value);


public: // QAbstractItemModel interface
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;


private: // slots
    void onLoadFinished();
    void onFileChanged(const QString& path);


private:
    class ImageLoadThread;
    std::unique_ptr<ImageLoadThread> _thread;
    QFileSystemWatcher               _watcher;
    bool                             _oneMore;
    const QString                    _path;
    int                              _maxCount;
    std::map<QString, QPixmap>       _imageMap;
};

} // namespace gm

