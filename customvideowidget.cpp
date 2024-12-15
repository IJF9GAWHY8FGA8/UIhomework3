#include "customvideowidget.h"
#include <QResizeEvent>

CustomVideoWidget::CustomVideoWidget(QWidget *parent)
    : QVideoWidget(parent)
{

}

void CustomVideoWidget::resizeEvent(QResizeEvent *event)
{
    emit resized(event->size().width());  // Emit the resized signal with the new width
    QVideoWidget::resizeEvent(event);  // Call the base class's resizeEvent
}
