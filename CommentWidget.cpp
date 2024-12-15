#include "CommentWidget.h"

CommentWidget::CommentWidget(QWidget *parent) : QWidget(parent), layout(new QVBoxLayout(this)) {
    listWidget = new QListWidget(this);
    layout->addWidget(listWidget);
}

void CommentWidget::loadComments(const QList<Comment> &comments) {
    listWidget->clear();
    foreach (const Comment &comment, comments) {
        listWidget->addItem(comment.text);
    }
}
