#include "animationmanager.h"
#include <QDebug>

AnimationManager::AnimationManager(QObject *parent)
    : QObject(parent),
    animationSpeed_(50),
    isAnimating_(false),
    productPosition_(250, 500),  // posición inicial visible
    isPaused_(false),
    pauseDuration_(100),       // Duración de la pausa en ciclos
    currentPauseTime_(0),
    currentPauseIndex_(0)      // Inicializa el índice de pausa en 0
{
    animationTimer_ = new QTimer(this);

    connect(animationTimer_, &QTimer::timeout,
            this, &AnimationManager::onAnimationFrame);

    boxPixmap_ = QPixmap("/home/fahern/Descargas/ProyectoSistemas/resources/caja/caja.png");

    // Inicializamos las posiciones donde queremos hacer las pausas
    pausePositions_ = {250, 500, 750, 1000, 1200};  // Puntos donde deseas hacer las pausas
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
    }
}

void AnimationManager::onAnimationFrame()
{
    // Si la animación está en pausa, no se mueve
    if (isPaused_) {
        currentPauseTime_++;  // Incrementamos el tiempo de pausa

        // Si se ha alcanzado la duración de la pausa, continuamos el movimiento
        if (currentPauseTime_ >= pauseDuration_) {
            isPaused_ = false;  // Desactiva la pausa
            currentPauseTime_ = 0;  // Reinicia el contador de pausa

            // Pasamos al siguiente índice de pausa
            currentPauseIndex_++;

            // Si hemos pasado por todas las pausas, reiniciamos
            if (currentPauseIndex_ >= pausePositions_.size()) {
                currentPauseIndex_ = 0;  // Reinicia el índice para comenzar nuevamente
            }
        }
        return;  // Sale para no mover la caja
    }

    // Movimiento horizontal
    productPosition_.rx() += 5;  // Mueve a la derecha

    // Verificamos si la caja ha llegado a la posición de pausa
    if (productPosition_.x() >= pausePositions_[currentPauseIndex_] &&
        productPosition_.x() <= pausePositions_[currentPauseIndex_] + 50 && !isPaused_) {
        isPaused_ = true;  // Activa la pausa
        pauseDuration_ = 100;  // Duración de la pausa en ciclos (ajusta este valor)
    }

    // Si la caja sale de la pantalla a la derecha, reiniciamos la posición
    if (productPosition_.x() > 1200) {
        productPosition_.setX(0);
    }

    emit positionChanged(productPosition_);
}

void AnimationManager::updatePosition(int, const QPoint &newPosition)
{
    productPosition_ = newPosition;
}

//
// ✔ ESTO reemplaza paintEvent() — método seguro
//
void AnimationManager::render(QPainter &painter)
{
    if (!boxPixmap_.isNull()) {
        // Ajusta la posición Y para mover la caja hacia arriba
        QPoint adjustedPosition = productPosition_;
        adjustedPosition.setY(productPosition_.y() - 125);  // Ajusta hacia arriba

        // Escala y dibuja la caja
        QPixmap scaled = boxPixmap_.scaled(200, 200, Qt::KeepAspectRatio);
        painter.drawPixmap(adjustedPosition, scaled);  // Dibuja la caja en la nueva posición ajustada
    }
}
