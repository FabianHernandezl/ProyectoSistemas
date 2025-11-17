#include "animationmanager.h"
#include <QDebug>

AnimationManager::AnimationManager(QObject *parent)
    : QObject(parent),
    animationSpeed_(50),
    isAnimating_(false),
    productPosition_(250, 500),   // INICIO INTENCIONAL EN LA ESTACIÓN 1
    isPaused_(false),
    pauseDuration_(100),
    currentPauseTime_(0),
    currentPauseIndex_(0)
{
    animationTimer_ = new QTimer(this);

    connect(animationTimer_, &QTimer::timeout,
            this, &AnimationManager::onAnimationFrame);

    boxPixmap_ = QPixmap("/home/fahern/Descargas/ProyectoSistemas/resources/caja/caja.png");

    // 🔥 5 estaciones de pausa
    pausePositions_ = {250, 500, 750, 1000, 1200};
}

AnimationManager::~AnimationManager()
{
}

void AnimationManager::startAnimations()
{
    if (!isAnimating_) {
        isAnimating_ = true;
        animationTimer_->start(animationSpeed_);
    }
}

void AnimationManager::stopAnimations()
{
    if (isAnimating_) {
        isAnimating_ = false;
        animationTimer_->stop();
        emit allWorkersStop();
    }
}

void AnimationManager::onAnimationFrame()
{
    // Si está detenido en una pausa
    if (isPaused_) {

        currentPauseTime_++;

        if (currentPauseTime_ >= pauseDuration_) {

            isPaused_ = false;
            currentPauseTime_ = 0;

            // DETENER AL TRABAJADOR QUE ACABA DE TERMINAR
            switch (currentPauseIndex_) {
            case 0: emit stop1Reached(); break;
            case 1: emit stop2Reached(); break;
            case 2: emit stop3Reached(); break;
            case 3: emit stop4Reached(); break;
            case 4: emit stop5Reached(); break;
            }

            currentPauseIndex_++;
        }


        return;
    }

    // Movimiento
    productPosition_.rx() += 5;

    // 🔥 ACTIVAR PAUSA CUANDO SE LLEGUE A UNA DE LAS 5 ESTACIONES
    if (currentPauseIndex_ < pausePositions_.size()) {

        int pauseX = pausePositions_[currentPauseIndex_];

        if (productPosition_.x() >= pauseX &&
            productPosition_.x() <= pauseX + 20)
        {
            isPaused_ = true;

            switch (currentPauseIndex_) {
            case 0: emit pause1Reached(); break;
            case 1: emit pause2Reached(); break;
            case 2: emit pause3Reached(); break;
            case 3: emit pause4Reached(); break;
            case 4: emit pause5Reached(); break;
            }
        }
    }

    // 🔄 Reinicio de la animación al final
    if (productPosition_.x() > 1300) {
        productPosition_.setX(250);
        currentPauseIndex_ = 0;
        emit allWorkersStop();
    }

    emit positionChanged(productPosition_);
}

void AnimationManager::updatePosition(int, const QPoint &newPos)
{
    productPosition_ = newPos;
}

void AnimationManager::render(QPainter &painter)
{
    if (!boxPixmap_.isNull()) {
        QPoint adjusted = productPosition_;
        adjusted.setY(productPosition_.y() - 125);  // Mantener caja arriba

        QPixmap scaled = boxPixmap_.scaled(200, 200, Qt::KeepAspectRatio);
        painter.drawPixmap(adjusted, scaled);
    }
}
