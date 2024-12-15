#ifndef PLAYLISTWIDGET_H
#define PLAYLISTWIDGET_H

#include <QWidget>

class QListWidget;
class QPushButton;

class PlaylistWidget : public QWidget {
    Q_OBJECT

public:
    PlaylistWidget(QWidget *parent = nullptr);

private slots:
    void addItem();
    void removeItem();
    void sortItems();

private:
    QListWidget *playlist;
    QPushButton *addButton;
    QPushButton *removeButton;
    QPushButton *sortButton;
};

#endif // PLAYLISTWIDGET_H

