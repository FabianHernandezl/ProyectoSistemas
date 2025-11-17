#include "buffer.h"
#include <QTime>

static QString ts()
{
    return QTime::currentTime().toString("hh:mm:ss.zzz");
}

Buffer::Buffer(int index, int capacity)
    : index_(index), capacity_(capacity)
{
    qDebug() << ts() << "🧱 Buffer creado -> Index" << index_
             << "Capacidad" << capacity_;
}

int Buffer::size() const
{
    QMutexLocker locker(&mutex_);
    return queue_.size();
}

void Buffer::insert(const Product &p)
{
    QMutexLocker locker(&mutex_);

    while (queue_.size() >= capacity_)
    {
        // Esperar hasta que haya espacio
        if (!notFull_.wait(&mutex_, 500))
        {
            qDebug() << ts() << "⚠️ [B" << index_
                     << "] Buffer lleno -> esperando...";
            return;
        }
    }

    queue_.enqueue(p);

    if (p.isPoison())
    {
        qDebug() << ts() << "💀 [B" << index_
                 << "] Insertado POISON PILL";
    }
    else
    {
        qDebug() << ts() << "📦 [B" << index_
                 << "] Insertado P" << p.id()
                 << "(" << queue_.size() << "/" << capacity_ << ")";
    }

    notEmpty_.wakeOne();
}

Product Buffer::remove()
{
    QMutexLocker locker(&mutex_);

    while (queue_.isEmpty())
    {
        notEmpty_.wait(&mutex_);
    }

    Product p = queue_.dequeue();

    if (p.isPoison())
    {
        qDebug() << ts() << "💀 [B" << index_
                 << "] Removido POISON PILL";
    }
    else
    {
        qDebug() << ts() << "📤 [B" << index_
                 << "] Removido P" << p.id()
                 << "(" << queue_.size() << "/" << capacity_ << ")";
    }

    notFull_.wakeOne();
    return p;
}

void Buffer::forceWake()
{
    QMutexLocker locker(&mutex_);
    notEmpty_.wakeAll();
    notFull_.wakeAll();
    qDebug() << ts() << "⚡ [B" << index_
             << "] ForceWake enviado a todos los hilos";
}
