#ifndef PRODUCT_H
#define PRODUCT_H

#include <QString>

class Product
{
public:
    Product();
    Product(int id, const QString &type);
    Product(bool poison);

    int id() const { return id_; }
    int getId() const { return id_; }     // ← Compatibilidad con tu código viejo
    QString type() const { return type_; }
    QString currentState() const { return currentState_; }

    bool isPoison() const { return poison_; }

    void advanceState(const QString &newState);

    QString info() const;

private:
    int id_;
    QString type_;
    bool poison_;
    QString currentState_;
};

#endif // PRODUCT_H
