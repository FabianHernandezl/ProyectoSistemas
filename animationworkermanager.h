#ifndef ANIMATIONWORKERMANAGER_H
#define ANIMATIONWORKERMANAGER_H

#include <QObject>
#include <QTimer>
#include <QPixmap>

class AnimationWorkerManager : public QObject
{
    Q_OBJECT

public:
    explicit AnimationWorkerManager(QObject *parent = nullptr);

    // Iniciar y detener la animación
    void start();
    void stop();

    // Establecer las imágenes del trabajador
    void setFrames(const QString &frameAPath, const QString &frameBPath);

    // Obtener el frame actual para dibujarlo
    QPixmap currentFrame() const;

signals:
    void frameChanged();   // Notifica a MainWindow para repintar

private slots:
    void onFrameUpdate();  // Alterna entre frameA y frameB

private:
    QTimer *timer_;        // Timer para la animación
    bool toggle_;          // Control del frame actual

    QPixmap frameA_;       // Frame 1 (quieto)
    QPixmap frameB_;       // Frame 2 (animado)
};

#endif // ANIMATIONWORKERMANAGER_H
