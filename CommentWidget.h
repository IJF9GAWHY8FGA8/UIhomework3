#ifndef COMMENTWIDGET_H
#define COMMENTWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout> // 包含QVBoxLayout头文件
#include "Comment.h"

class CommentWidget : public QWidget {
    Q_OBJECT

public:
    explicit CommentWidget(QWidget *parent = nullptr);

    void loadComments(const QList<Comment> &comments);

private:
    QListWidget *listWidget;
    QVBoxLayout *layout; // 定义布局为成员变量
};

#endif // COMMENTWIDGET_H
