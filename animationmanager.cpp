#include "animationmanager.h"
#include <QDebug>

AnimationManager::AnimationManager(QObject *parent)
    : QObject(parent),
    animationSpeed_(50),
    isAnimating_(false),
    productPosition_(0, 500)  // posición inicial visible
{
    animationTimer_ = new QTimer(this);

    connect(animationTimer_, &QTimer::timeout,
            this, &AnimationManager::onAnimationFrame);

    boxPixmap_ = QPixmap(":/resources/caja.png");
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
    // movimiento simple
    productPosition_.rx() += 5;

    if (productPosition_.x() > 1400)
        productPosition_.setX(0);

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
        painter.drawPixmap(productPosition_, boxPixmap_.scaled(120, 120, Qt::KeepAspectRatio));
    }
}
