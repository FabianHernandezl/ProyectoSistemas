#ifndef BUFFER_H
#define BUFFER_H

#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QDebug>
#include "product.h"

class Buffer {
public:
    Buffer(int index, int capacity);
    ~Buffer() = default;

    void insert(const Product &p);
    Product remove();
    void forceWake();

    int size() const;
    int capacity() const { return capacity_; }
    int index() const { return index_; }

private:
    int index_;
    int capacity_;
    mutable QMutex mutex_;
    QWaitCondition notEmpty_;
    QWaitCondition notFull_;
    QQueue<Product> queue_;
};

#endif // BUFFER_H
