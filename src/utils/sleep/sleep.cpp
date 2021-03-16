#include "utils/sleep/sleep.h"


namespace Sleep {
    

    void callBack () {

    }


    void loopSleep() {

        static uint32_t lastSleep = 0;

        if (millis() - lastSleep >= 100 && !UserInterface::interfaceActivity()) {
            lastSleep = millis();

            gpio_wakeup_enable(gpio_num_t(DIO1), GPIO_INTR_HIGH_LEVEL);
            esp_sleep_enable_gpio_wakeup();
            gpio_hold_en(gpio_num_t(TXEN));
            gpio_hold_en(gpio_num_t(RXEN));
            gpio_hold_en(gpio_num_t(NSS));
            gpio_hold_en(gpio_num_t(NRESET));
            touchAttachInterrupt(TOUCHINPUTPIN1, callBack, UserInterface::touchThreshold(0));
            touchAttachInterrupt(TOUCHINPUTPIN2, callBack, UserInterface::touchThreshold(1));
            touchAttachInterrupt(TOUCHINPUTPIN3, callBack, UserInterface::touchThreshold(2));
            esp_sleep_enable_touchpad_wakeup();
            Serial.println("CPU Sleeping");
            esp_sleep_enable_timer_wakeup(5*1000000L);
            delay(10);
            esp_light_sleep_start();
            Serial.println("CPU Awake");

            gpio_hold_dis(gpio_num_t(TXEN));
            gpio_hold_dis(gpio_num_t(RXEN));
            gpio_hold_dis(gpio_num_t(NSS));
            gpio_hold_dis(gpio_num_t(NRESET));

            lastSleep = millis();

        }

    }

}
