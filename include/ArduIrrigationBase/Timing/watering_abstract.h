#ifndef WATERING_ABSTRACT_H
#define WATERING_ABSTRACT_H


#include "KraftKontrol/utils/date_time.h"
#include "KraftKontrol/utils/list.h"



class Watering_Abstract {
private:

    static List<Watering_Abstract*>& wateringList_();


protected:

    Watering_Abstract();

    virtual ~Watering_Abstract();


public:

    static const List<Watering_Abstract*>& getWateringTimeList();

    virtual TimeDate getNextWateringTime() const = 0;


};


#endif