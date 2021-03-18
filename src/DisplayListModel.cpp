#include "DisplayListModel.h"

#include "QGuiApplication"


namespace gm {

DisplayListModel::DisplayListModel()
{
    QCoreApplication *app = QGuiApplication::instance();
    auto guiApp = dynamic_cast<QGuiApplication*>(app);
    if (!guiApp)
        std::exit(EXIT_SUCCESS);


    for (QScreen* screen: QGuiApplication::screens())
    {
        auto name = QString("%1 - (%2 x %3)")
                .arg(screen->name())
                .arg(screen->size().width())
                .arg(screen->size().height());

        _screens.push_back({screen, name});
        connect(screen, &QObject::destroyed,
                this,   &DisplayListModel::onScreenDestroyed );
    }
}

QPixmap DisplayListModel::capture(const int loc)
{
    if (_screens.size() < static_cast<unsigned>(loc) ||
            loc < 0)
    {
        return {};
    }

    if (auto screen = _screens.at(loc).handle)
    {
        return screen->grabWindow(0);
    }

    return {};
}

int DisplayListModel::rowCount(const QModelIndex &) const
{
    return _screens.size();
}

QVariant DisplayListModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        const auto &screen = _screens.at(index.row());
        return screen.name;
    }

    return {};
}

void DisplayListModel::onScreenDestroyed(QObject *obj)
{
    auto result = std::find_if(
                _screens.begin(),
                _screens.end(),
                [&](const Screen& s) {
        return s.handle == obj;
    });

    if (result != _screens.end())
    {
        result->handle = nullptr;
    }
}

} // namespace gm
