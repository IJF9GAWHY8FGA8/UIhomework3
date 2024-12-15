#include "CommentManager.h"

void CommentManager::addComment(const Comment &comment) {
    comments.append(comment);
}

QList<Comment> CommentManager::getComments() const {
    return comments;
}
