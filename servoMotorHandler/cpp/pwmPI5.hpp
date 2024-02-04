#include <stdint.h>
#include <iostream>

//the RP1 doc gives byte offsets: number of bytes after which a register can be found
//working with 32 bit addresses -> divide by 4 to obtain the 32 bit offset

//offsets
#define IO_BLOCK                0x0d0000/4
#define IO_BLOCK_PADS           0x0f0000/4
#define PWM0                    0x098000/4
#define CLK_SYS                 0x018000/4

//PINS offsets
#define GPIO12_CTRL             0x000064/4
#define GPIO12_PAD_CTRL         0x000034/4
#define GPIO13_CTRL             0x00006C/4
#define GPIO13_PAD_CTRL         0x000038/4
#define GPIO14_CTRL             0x000074/4
#define GPIO14_PAD_CTRL         0x00003C/4
#define GPIO15_CTRL             0x00007C/4
#define GPIO15_PAD_CTRL         0x000040/4
    
#define GPIO18_CTRL             0x000094/4
#define GPIO18_PAD_CTRL         0x00004C/4
#define GPIO19_CTRL             0x00009C/4
#define GPIO19_PAD_CTRL         0x000050/4


//PWM PROPERTIES offsets
//CLK_PWM0 registers, found in https://github.com/raspberrypi/linux/blob/rpi-6.1.y/drivers/clk/clk-rp1.c
#define CLK_PWM0_CTRL               0x74/4
#define CLK_PWM0_DIV_INT		    0x78/4
#define CLK_PWM0_DIV_FRAC		    0x7c/4
#define CLK_PWM0_SEL		        0x80/4

#define CHAN0_CTRL                  0x14/4
#define CHAN1_CTRL                  0x24/4
#define CHAN2_CTRL                  0x34/4
#define CHAN3_CTRL                  0x44/4

#define CHAN_RANGE                       1
#define CHAN_PHASE                       2
#define CHAN_DUTY                        3

//NO IDEA WHERE THIS COMES FROM
#define CLOCK                         30.3  //MHz, VALUE OBTAINED BY READING THE REGISTER USING PWM THROUGH sysfs

class pwmPI5 {

    public:
        pwmPI5(uint8_t);
        bool setDutyCycle(uint16_t);
        ~pwmPI5();
    private:

        uint8_t pin_ctrl = GPIO12_CTRL;
        uint8_t pin_pad_ctrl = GPIO12_PAD_CTRL;
        uint8_t channel = 0;

        uint32_t* base_virtual_addr = nullptr;
        uint32_t* io_block = nullptr;
        uint32_t* io_block_pads = nullptr;
        uint32_t* clk_sys = nullptr;
        uint32_t* pwm = nullptr;

        bool mapIO();
        void unmapIO();
        bool setPIN();
        bool unsetPIN();
};

//python wrapping
#ifdef PYTHON_WRAPPER
extern "C" {
    pwmPI5* pwmPI5_new(uint8_t pin) { return new pwmPI5(pin); }
    bool setDutyCycle(pwmPI5* obj, uint16_t duty_cycle_ms) { return obj->setDutyCycle(duty_cycle_ms); }
    void pwmPI5_delete(pwmPI5* obj) { delete obj; }
}
#endif
