#include "MainWindow.h"
#include "./ui_MainWindow.h"

#include <filesystem>
#include <QDir>
#include <QFileDialog>
#include <QDesktopServices>
#include <QDateTime>
#include <QScrollEvent>
#include <QScrollBar>
#include <QUrl>
#include <QMessageBox>
#include "CaptureManager.h"
#include "ScreenCapture.h"
#include "CameraListModel.h"
#include "DisplayListModel.h"
#include "ImageListModel.h"



namespace gm {

namespace {
constexpr auto keyDefaultTargetType = "target_type";
constexpr auto keyDefaultTarget     = "target";
constexpr auto keyImageListMax      = "image_max";
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _settings("settings.ini", QSettings::IniFormat)
    , _captureManager(new CaptureManager)
{
    ui->setupUi(this);
    loadSettings();
    setupSignals();

    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 0);

    _scene.addItem(&_pixmapItem);
    ui->graphicsView->setScene(&_scene);
    ui->imageListView->installEventFilter(this);

    {
        const int targetType = _settings.value(keyDefaultTargetType, 0).toInt();
        const int target = _settings.value(keyDefaultTarget, 0).toInt();

        if (targetType == ui->typeComboBox->currentIndex())
            onCaptureTypeChanged(targetType);
        else
            ui->typeComboBox->setCurrentIndex(targetType);

        if (target == ui->targetComboBox->currentIndex())
            onCaptureTargetChanged(target);
        else
            ui->targetComboBox->setCurrentIndex(target);
    }

    onSaveDirectoryChanged();
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}


bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->imageListView
            && event->type() == QEvent::Wheel)
    {
        auto        we  = static_cast<QWheelEvent*>(event);
        QScrollBar *bar = ui->imageListView->horizontalScrollBar();

        int value = we->angleDelta().y();
        int current = bar->value();
        if (value != 0)
        {
            bar->setValue(current+value);
        }
    }

    return false;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    ui->graphicsView->fitInView(_capturedImage.rect(), Qt::KeepAspectRatio);
}

void MainWindow::shot()
{
    _captureManager->capture();
}

void MainWindow::referencePath()
{
    QString path = QFileDialog::getExistingDirectory(
                this,
                tr("Save directory"),
                ui->pathEdit->text()
                );
    if (!path.isEmpty())
        ui->pathEdit->setText(path);
}

void MainWindow::openFolder()
{
    openPath(ui->pathEdit->text());
}

void MainWindow::saveAs()
{
    if (_capturedImage.isNull())
        return;


    QString path = QFileDialog::getSaveFileName(
                this,
                windowTitle(),
                ui->pathEdit->text(),
                tr("Images (*.jpg *png)")
                );
    if (path.isEmpty())
        return;

    _capturedImage.save(path);
}

void MainWindow::reload()
{
    _captureManager->reload();
}

void MainWindow::onTargetModelChanged(QAbstractItemModel *model)
{
    ui->targetComboBox->setModel(model);
}

void MainWindow::onImageCaptured(const QImage &frame)
{
    _capturedImage = frame;
    _pixmapItem.setPixmap(QPixmap::fromImage(_capturedImage));
    _scene.setSceneRect(0, 0, _capturedImage.width(), _capturedImage.height());
    ui->graphicsView->fitInView(_capturedImage.rect(), Qt::KeepAspectRatio);

    ui->saveAsButton->setEnabled(true);

    if (!ui->withSaveCheckBox->isChecked())
        return;

    QString path = makeFilePath();
    if (frame.save(path))
    {
        ui->statusbar->showMessage(tr("Saved: ")+path, 5000);
    }
    else
    {
        ui->statusbar->showMessage(tr("Can't saved: ")+path, 5000);
    }

}

void MainWindow::onCaptureTypeChanged(int loc)
{
    static const std::map<int, CaptureManager::TargetType> types = {
        { -1, CaptureManager::None },
        {  0, CaptureManager::Display },
        {  1, CaptureManager::Camera },
    };

    _captureManager->setTargetType(types.at(loc));
}

void MainWindow::onCaptureTargetChanged(int loc)
{
    _captureManager->setTarget(loc);
}

void MainWindow::onSaveDirectoryChanged()
{
    auto path = ui->pathEdit->text().toStdU16String();
    if (std::filesystem::exists(path))
    {
        _imageListModel.reset(
                    new ImageListModel(ui->pathEdit->text(),
                                       ui->maxSpinBox->value() )
                    );
        ui->imageListView->setModel(_imageListModel.get());
        ui->imageListView->verticalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
        ui->imageListView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        ui->imageListView->resizeColumnsToContents();
    }
}

void MainWindow::onCustomContextMenuRequested(const QPoint &pos)
{
    auto selected = ui->imageListView->selectionModel()->selectedColumns(1);
    if (selected.size() == 0)
        return;

    QMenu menu(ui->imageListView);
    QAction* openAction = menu.addAction(tr("&Open"));
    QAction* deleteAction = menu.addAction(tr("&Delete"));
    openAction->setEnabled(selected.size() == 1);

    QAction* result = menu.exec(ui->imageListView->mapToGlobal(pos));
    if (result == openAction)
    {
        auto path = selected[0].data(ImageListModel::UserRole_Path).toString();
        openPath(path);
    }
    else if (result == deleteAction)
    {
        auto button = QMessageBox::question(
                    this,
                    this->windowTitle(),
                    tr("Are you sure you want to delete?")
                    );
        if (button != QMessageBox::Yes)
            return;

        for (const QModelIndex& index: selected)
        {
            QFile file(index.data(ImageListModel::UserRole_Path).toString());
            file.remove();
        }
    }
}

void MainWindow::onMaxCountChanged(const int value)
{
    _imageListModel->setMaxCount(value);
}

void MainWindow::setupSignals()
{
    connect(_captureManager.get(),  &CaptureManager::modelChanged,
            this,                   &MainWindow::onTargetModelChanged );
    connect(_captureManager.get(),  &CaptureManager::imageCaptured,
            this,                   &MainWindow::onImageCaptured );
    connect(ui->shotButton,         &QAbstractButton::clicked,
            this,                   &MainWindow::shot );
    connect(ui->referenceButton,    &QAbstractButton::clicked,
            this,                   &MainWindow::referencePath );
    connect(ui->openFolderButton,   &QAbstractButton::clicked,
            this,                   &MainWindow::openFolder );
    connect(ui->reloadButton,       &QAbstractButton::clicked,
            this,                   &MainWindow::reload );
    connect(ui->saveAsButton,       &QAbstractButton::clicked,
            this,                   &MainWindow::saveAs );
    connect(ui->pathEdit,           &QLineEdit::textChanged,
            this,                   &MainWindow::onSaveDirectoryChanged );
    connect(ui->typeComboBox,       qOverload<int>(&QComboBox::currentIndexChanged),
            this,                   &MainWindow::onCaptureTypeChanged );
    connect(ui->targetComboBox,     qOverload<int>(&QComboBox::currentIndexChanged),
            this,                   &MainWindow::onCaptureTargetChanged );
    connect(ui->imageListView,      &QTableView::customContextMenuRequested,
            this,                   &MainWindow::onCustomContextMenuRequested );
    connect(ui->maxSpinBox,         qOverload<int>(&QSpinBox::valueChanged),
            this,                   &MainWindow::onMaxCountChanged );
    connect(ui->imageListView->horizontalScrollBar(), &QScrollBar::rangeChanged,
            [this] (int , int max) {
        ui->imageListView->horizontalScrollBar()->setValue(max);
    });
}

QString MainWindow::makeFilePath() const
{
    auto datetime = QDateTime::currentDateTime();
    QString datetimeStr = datetime.toString(" yyyy-MM-dd hh_mm_ss");

    return ui->pathEdit->text()
            + QDir::separator()
            + ui->nameEdit->text()
            + datetimeStr + ".jpg";
}

void MainWindow::openPath(const QString &path)
{
    if (path.isEmpty())
        return;

    QFileInfo info(path);
    if (info.exists())
    {
        QDesktopServices::openUrl(
                    QUrl::fromLocalFile(path)
                    );
    }
    else
    {
        ui->statusbar->showMessage(tr("Not exist: ")+path, 5000);
    }
}

void MainWindow::saveSettings()
{
    _settings.setValue("path", ui->pathEdit->text());
    _settings.setValue("name", ui->nameEdit->text());
    _settings.setValue("geometry", saveGeometry());
    _settings.setValue("splitter", ui->splitter->saveState());

    _settings.setValue(keyDefaultTargetType, ui->typeComboBox->currentIndex());
    _settings.setValue(keyDefaultTarget, ui->targetComboBox->currentIndex());
    _settings.setValue(keyImageListMax, ui->maxSpinBox->value());
}

void MainWindow::loadSettings()
{
    ui->pathEdit->setText(_settings.value("path").toString());
    ui->nameEdit->setText(_settings.value("name").toString());
    ui->maxSpinBox->setValue(_settings.value(keyImageListMax, 10).toInt());
    restoreGeometry(_settings.value("geometry").toByteArray());
    ui->splitter->restoreState(_settings.value("splitter").toByteArray());
}


} // namespace gm
