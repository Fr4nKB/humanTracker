#include <unistd.h>
#include "pwmPI5.hpp"

int main() {
    pwmPI5 pwm(12);
    pwm.setDutyCycle(500);
    sleep(1);
    pwm.setDutyCycle(1500);
    sleep(1);
    pwm.setDutyCycle(2500);
    sleep(1);
    pwm.setDutyCycle(500);
    sleep(2);
    for (int i = 50; i <= 250; i++){
        pwm.setDutyCycle(i*10);
        usleep(10000);
    }
    return 0;
}