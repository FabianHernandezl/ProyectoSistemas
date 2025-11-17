#include "animationworkermanager.h"
#include <QDebug>

AnimationWorkerManager::AnimationWorkerManager(QObject *parent)
    : QObject(parent),
    toggle_(false)
{
    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, &AnimationWorkerManager::onFrameUpdate);
}

void AnimationWorkerManager::setFrames(const QString &frameAPath, const QString &frameBPath)
{
    frameA_ = QPixmap(frameAPath);
    frameB_ = QPixmap(frameBPath);

    if (frameA_.isNull() || frameB_.isNull()) {
        qDebug() << "❌ Error: No se pudieron cargar las imágenes de animación";
    }
}

void AnimationWorkerManager::start()
{
    toggle_ = false;      // Volver al frame inicial
    timer_->start(500);   // Cambia cada 0.5 segundos (ajustable)
}

void AnimationWorkerManager::stop()
{
    timer_->stop();
    toggle_ = false;      // Vuelve a frame A cuando se detiene
}

void AnimationWorkerManager::onFrameUpdate()
{
    toggle_ = !toggle_;   // Alterna entre A y B
    emit frameChanged();  // Pide repintar la ventana
}

QPixmap AnimationWorkerManager::currentFrame() const
{
    return toggle_ ? frameB_ : frameA_;
}
