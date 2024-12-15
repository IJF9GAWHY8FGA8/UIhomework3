#include "AddCommentWidget.h"

AddCommentWidget::AddCommentWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    commentEdit = new QLineEdit(this);
    addCommentButton = new QPushButton("Add Comment", this);

    layout->addWidget(commentEdit);
    layout->addWidget(addCommentButton);

    connect(addCommentButton, &QPushButton::clicked, this, &AddCommentWidget::onAddCommentClicked);
}

void AddCommentWidget::onAddCommentClicked() {
    QString commentText = commentEdit->text();
    if (!commentText.isEmpty()) {
        emit addCommentSignal(Comment(commentText));
        commentEdit->clear();
    }
}
