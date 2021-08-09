#pragma once

#include <memory>
#include <QAbstractItemModel>
#include <QSettings>
#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>



namespace gm {

class CaptureManager;
class CameraListModel;
class DisplayListModel;
class ImageListModel;
class MainWindowFunc;
namespace Ui { class MainWindow; }


class MainWindow
        : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


public: // QObject interface
    bool eventFilter(QObject *watched, QEvent *event) override;

protected: // QWidget interface
    void resizeEvent(QResizeEvent *event) override;


private: // slots
    void shot();
    void referencePath();
    void openFolder();
    void saveAs();
    void reload();
    void onTargetModelChanged(QAbstractItemModel* model);
    void onImageCaptured(const QImage& frame);
    void onCaptureTypeChanged(int loc);
    void onCaptureTargetChanged(int loc);
    void onSaveDirectoryChanged();
    void onCustomContextMenuRequested(const QPoint& pos);
    void onDoubleClicked(const QModelIndex &index);
    void onMaxCountChanged(const int value);


private:
    void    setupSignals();
    QString makeFilePath() const;
    void    openPath    (const QString &path);
    void    saveSettings();
    void    loadSettings();

    Ui::MainWindow*     ui;
    QGraphicsScene      _scene;
    QGraphicsPixmapItem _pixmapItem;
    QImage              _capturedImage;

    QSettings                       _settings;
    std::unique_ptr<CaptureManager> _captureManager;
    std::unique_ptr<ImageListModel> _imageListModel;
};

} // namespace gm
