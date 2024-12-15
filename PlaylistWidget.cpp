#include "PlaylistWidget.h"
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QInputDialog>

PlaylistWidget::PlaylistWidget(QWidget *parent) : QWidget(parent) {
    auto *mainLayout = new QVBoxLayout(this);

    playlist = new QListWidget(this);
    playlist->addItem("默认专辑");
    playlist->addItem("此电脑");
    playlist->addItem("+ 新建专辑");

    mainLayout->addWidget(playlist);

    auto *buttonLayout = new QHBoxLayout();
    addButton = new QPushButton("添加", this);
    removeButton = new QPushButton("删除", this);
    sortButton = new QPushButton("排序", this);

    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(removeButton);
    buttonLayout->addWidget(sortButton);

    mainLayout->addLayout(buttonLayout);

    connect(addButton, &QPushButton::clicked, this, &PlaylistWidget::addItem);
    connect(removeButton, &QPushButton::clicked, this, &PlaylistWidget::removeItem);
    connect(sortButton, &QPushButton::clicked, this, &PlaylistWidget::sortItems);
}

void PlaylistWidget::addItem() {
    bool ok;
        QString text = QInputDialog::getText(this, tr("添加项目"),
                                             tr("项目名称:"), QLineEdit::Normal,
                                             "", &ok);
        if (ok && !text.isEmpty()) {
            playlist->addItem(text);
        }
}

void PlaylistWidget::removeItem() {
    QListWidgetItem *item = playlist->currentItem();
        if (item) {
            delete playlist->takeItem(playlist->row(item));
        }
}

void PlaylistWidget::sortItems() {
    playlist->sortItems();
}
