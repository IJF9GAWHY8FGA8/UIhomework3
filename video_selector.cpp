#include "video_selector.h"

VideoSelector::VideoSelector(const std::vector<TheButtonInfo>& videos, QWidget* parent) : QWidget(parent), videos(videos) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    videoList = new QListWidget(this);
    layout->addWidget(videoList);

    for (const auto& video : videos) {
        QListWidgetItem* item = new QListWidgetItem(videoList);
        item->setText(video.url->toString());
    }

    connect(videoList, &QListWidget::itemClicked, this, &VideoSelector::onVideoSelected);
}

void VideoSelector::selectVideo(const QUrl& url) {
    for (int i = 0; i < videoList->count(); ++i) {
        QListWidgetItem* item = videoList->item(i);
        if (item->text() == url.toString()) {
            videoList->setCurrentItem(item);
            break;
        }
    }
}

void VideoSelector::onVideoSelected(QListWidgetItem* item) {
    for (const auto& video : videos) {
        if (video.url->toString() == item->text()) {
            emit videoSelected(*video.url);
            break;
        }
    }
}
