#include "watering_abstract.h"



Watering_Abstract::Watering_Abstract() {

    wateringList_().append(this);

}

Watering_Abstract::~Watering_Abstract() {

    wateringList_().removeAllEqual(this);

}


List<Watering_Abstract*>& Watering_Abstract::wateringList_() {

    static List<Watering_Abstract*> wateringList;

    return wateringList;

}


const List<Watering_Abstract*>& Watering_Abstract::getWateringTimeList() {
    return wateringList_();
}
