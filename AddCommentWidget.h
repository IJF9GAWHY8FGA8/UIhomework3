#ifndef ADDCOMMENTWIDGET_H
#define ADDCOMMENTWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include "Comment.h"

class AddCommentWidget : public QWidget {
    Q_OBJECT

public:
    explicit AddCommentWidget(QWidget *parent = nullptr);

signals:
    void addCommentSignal(const Comment& comment);

private slots:
    void onAddCommentClicked();

private:
    QLineEdit *commentEdit;
    QPushButton *addCommentButton;
};

#endif // ADDCOMMENTWIDGET_H
