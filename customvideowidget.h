#ifndef CUSTOMVIDEOWIDGET_H
#define CUSTOMVIDEOWIDGET_H

#include <QVideoWidget>

class CustomVideoWidget : public QVideoWidget
{
    Q_OBJECT
public:
    explicit CustomVideoWidget(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;  // Add this line to declare the resizeEvent function

signals:
    void resized(int newWidth);
};

#endif // CUSTOMVIDEOWIDGET_H
