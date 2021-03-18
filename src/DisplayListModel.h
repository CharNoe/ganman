#pragma once

#include <QAbstractListModel>
#include <QScreen>
#include <QPixmap>


namespace gm {

class DisplayListModel
        : public QAbstractListModel
{
public:
    DisplayListModel();


    QPixmap capture(const int loc);


public: // QAbstractItemModel interface
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;


private:
    struct Screen
    {
        QScreen *handle;
        QString name;
    };


private:
    void onScreenDestroyed(QObject *obj);

    std::vector<Screen> _screens;
};

} // namespace gm

