#include "product.h"

Product::Product() : id_(-1), type_("None"), poison_(false), currentState_("Created") {}

Product::Product(int id, const QString &type)
    : id_(id), type_(type), poison_(false), currentState_("Created") {}

Product::Product(bool poison)
    : id_(-1), type_("Poison"), poison_(poison), currentState_("Stop") {}

void Product::advanceState(const QString &newState) {
    currentState_ = newState;
}

QString Product::info() const {
    return QString("Product{id=%1, type=%2, state=%3}")
    .arg(id_).arg(type_).arg(currentState_);
}
