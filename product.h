#ifndef PRODUCT_H
#define PRODUCT_H

#include <QString>

class Product {
public:
    Product();
    Product(int id, const QString &type);
    explicit Product(bool poison);

    bool isPoison() const { return poison_; }

    int getId() const { return id_; }
    int id() const { return id_; }
    QString type() const { return type_; }

    void advanceState(const QString &newState);
    QString info() const;

    static Product poisonPill() { return Product(true); }

private:
    int id_;
    QString type_;
    bool poison_;
    QString currentState_;
};

#endif // PRODUCT_H
