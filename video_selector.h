#ifndef VIDEO_SELECTOR_H
#define VIDEO_SELECTOR_H

#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QUrl>
#include "the_button.h"

class VideoSelector : public QWidget {
    Q_OBJECT

public:
    explicit VideoSelector(const std::vector<TheButtonInfo>& videos, QWidget* parent = nullptr);
    void selectVideo(const QUrl& url);

signals:
    void videoSelected(const QUrl& url);

private slots:
    void onVideoSelected(QListWidgetItem* item);

private:
    QListWidget* videoList;
    std::vector<TheButtonInfo> videos;
};

#endif // VIDEO_SELECTOR_H
