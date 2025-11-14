#include "buffer.h"
#include <QTime>

static QString ts()
{
    return QTime::currentTime().toString("\"hh:mm:ss.zzz\"");
}

Buffer::Buffer(int index, int capacity)
    : index_(index), capacity_(capacity)
{
    qDebug() << ts() << "🧱 Buffer created -> index" << index_;
}

int Buffer::size() const
{
    QMutexLocker lock(&mutex_);
    return queue_.size();
}

void Buffer::insert(const Product &p)
{
    QMutexLocker lock(&mutex_);
    while (queue_.size() >= capacity_) {
        if (!notFull_.wait(&mutex_, 500)) {
            // Si pasan 500 ms sin espacio, liberamos el hilo
            qDebug() << ts() << "⚠️ [B" << index_ << "] Waiting for space...";
            return;
        }
    }

    queue_.enqueue(p);
    if (p.isPoison()) {
        qDebug() << ts() << "💀 [B" << index_ << "] Inserted POISON PILL";
    } else {
        qDebug() << ts() << "📦 [B" << index_ << "] Inserted P" << p.id()
            << "(" << queue_.size() << "/" << capacity_ << ")";
    }
    notEmpty_.wakeOne();
}


Product Buffer::remove()
{
    QMutexLocker lock(&mutex_);
    while (queue_.isEmpty()) {
        notEmpty_.wait(&mutex_);
    }

    Product p = queue_.dequeue();
    if (p.isPoison()) {
        qDebug() << ts() << "💀 [B" << index_ << "] Removed POISON PILL";
    } else {
        qDebug() << ts() << "📤 [B" << index_ << "] Removed  P" << p.id()
            << "(" << queue_.size() << "/" << capacity_ << ")";
    }
    notFull_.wakeOne();
    return p;
}

void Buffer::forceWake()
{
    QMutexLocker lock(&mutex_);
    notEmpty_.wakeAll();  // Despertar a los hilos bloqueados esperando en remove().
    notFull_.wakeAll();   // Despertar a los hilos bloqueados esperando en insert().
    qDebug() << ts() << "Force waking all threads in buffer" << index_;
}
