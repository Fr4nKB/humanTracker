#include "pwmPI5.hpp"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>

/*
The logic of this code can be extrapolated by the official RP1's doc
https://datasheets.raspberrypi.com/rp1/rp1-peripherals.pdf

Thanks to danjperron, my code is highly inspired by his.
You can find it at https://github.com/danjperron/Pi5PWM_HARDWARE
*/

pwmPI5::pwmPI5(uint8_t pin) {
    switch(pin) {
        case 12:
            this->pin_pad_ctrl = GPIO12_PAD_CTRL;
            this->pin_ctrl = GPIO12_CTRL;
            this->channel = CHAN0_CTRL;
            break;
        case 13:
            this->pin_pad_ctrl = GPIO13_PAD_CTRL;
            this->pin_ctrl = GPIO13_CTRL;
            this->channel = CHAN1_CTRL;
            break;
        case 14:
            this->pin_pad_ctrl = GPIO14_PAD_CTRL;
            this->pin_ctrl = GPIO14_CTRL;
            this->channel = CHAN2_CTRL;
            break;
        case 15:
            this->pin_pad_ctrl = GPIO15_PAD_CTRL;
            this->pin_ctrl = GPIO15_CTRL;
            this->channel = CHAN3_CTRL;
            break;
        case 18:
            this->pin_pad_ctrl = GPIO18_PAD_CTRL;
            this->pin_ctrl = GPIO18_CTRL;
            this->channel = CHAN2_CTRL;
            break;
        case 19:
            this->pin_pad_ctrl = GPIO19_PAD_CTRL;
            this->pin_ctrl = GPIO19_CTRL;
            this->channel = CHAN3_CTRL;
            break;
        default:
            std::cout<<"Invalid pin choice"<<std::endl;
            throw;
    }

    if(!this->mapIO()) throw;
    if(!this->setPIN()) throw;

}

pwmPI5::~pwmPI5() {
    this->unsetPIN();
    this->unmapIO();
}

bool pwmPI5::mapIO() {
    // access sysfs file that contains host addresses of PCI resources
    // this is needed because of virtual memory, the address of sysinfo in the docs is physical
    int fd = open("/sys/bus/pci/devices/0000:01:00.0/resource1", O_RDWR | O_SYNC);
    if (fd == -1) {
        std::cout<<"Couldn't fetch file to access physical memory"<<std::endl;
        return false;
    }

    // map the physical address to virtual memory:
    // addr = NULL -> chose page - aligned address at which the mapping will be created
    // length = file dimension in byte
    // PROTection: can read and write
    // MAP_SHARED: changes are visibile to other processes
    // offset = 0
    base_virtual_addr = (uint32_t*)mmap(NULL, 4194304, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (base_virtual_addr == MAP_FAILED) {
        std::cout<<"Couldn't map physical memory to virtual"<<std::endl;
        std::cout<<errno<<std::endl;
        return false;
    }

    int ret = close(fd);
    if(ret == -1) {
        std::cout<<"Couldn't close file to access physical memory"<<std::endl;
        return false;
    }

    this->io_block = base_virtual_addr + IO_BLOCK;
    this->io_block_pads = base_virtual_addr + IO_BLOCK_PADS;
    this->clk_sys = base_virtual_addr + CLK_SYS;
    this->pwm = base_virtual_addr + PWM0;
    
    //set system clock
    this->clk_sys[CLK_PWM0_CTRL] = 0x11000800;  //VALUE OBTAINED BY READING THE REGISTER USING PWM THROUGH sysfs
    this->clk_sys[CLK_PWM0_DIV_INT] = 1;        //clock prev setup is going to be divided by 1 so full clock is used
    this->clk_sys[CLK_PWM0_DIV_FRAC] = 0;
    this->clk_sys[CLK_PWM0_SEL] = 1;
    return true;
}

void pwmPI5::unmapIO() {
    this->io_block = nullptr;
    this->io_block_pads = nullptr;
    this->pwm = nullptr;
    munmap(base_virtual_addr, 4194304);
}

bool pwmPI5::setPIN() {
    try {
        //[31:5] -> leave unchanged, [4:0] -> function selection
        auto temp = this->io_block[this->pin_ctrl];

        temp &= 0xFFFFFFE0;
        if(this->pin_ctrl == GPIO18_CTRL || this->pin_ctrl == GPIO19_CTRL) temp |= 0x3;

        this->io_block[this->pin_ctrl] = temp;

        /*
        Output disable = OFF
        Input Enable = ON
        Drive = 4 mA
        Pull Up Enable = OFF
        Pull Down Enable = ON
        Schmitt Trigger = ON
        Slew rate = slow
        */
        temp = this->io_block_pads[this->pin_pad_ctrl];
        temp &= 0xFFFFFF00;
        temp |= 0x56;
        this->io_block_pads[this->pin_pad_ctrl] = temp;

        //set FIFO_POP_MASK to 1 and MODE to Trailing edge mark space
        this->pwm[this->channel] = 0x81;
        //range is obtained by multiplying frequency of clock (30.3 MHz) and period (20000 us)
        this->pwm[this->channel + CHAN_RANGE] = uint32_t(CLOCK * 20000);
        return true;
    }
    catch(const std::exception &exc) {
        std::cout<<"Couldn't set pin: ";
        std::cerr << exc.what();
        return false;
    }
}


bool pwmPI5::unsetPIN() {
    try {
        //reset function
        auto temp = this->io_block[this->pin_ctrl];
        temp |= 0x1F;
        this->io_block[this->pin_ctrl] = temp;

        //reset to default
        temp = this->io_block_pads[this->pin_pad_ctrl];
        temp &= 0xFFFFFF00;
        temp |= 0x96;
        this->io_block_pads[this->pin_pad_ctrl] = temp;
        return true;
    }
    catch(const std::exception &exc) {
        std::cout<<"Couldn't unset pin: ";
        std::cerr << exc.what();
        return false;
    }
}


bool pwmPI5::setDutyCycle(uint16_t duty_cycle_ms) {
    try {
        if(duty_cycle_ms < 500 || duty_cycle_ms > 2500) throw;

        //first set duty cycle
        this->pwm[this->channel + CHAN_DUTY] = CLOCK * duty_cycle_ms;

        //then set PWM0.GLOBAL_CTRL.CHAX_EN to 1
        auto temp = this->pwm[0];
        temp |= 0x80000001;     //enable and set_update
        this->pwm[0] = temp;
        return true;
    }
    catch(const std::exception &exc) {
        std::cout<<"Couldn't set duty cycle: ";
        std::cerr << exc.what();
        return false;
    }
}

