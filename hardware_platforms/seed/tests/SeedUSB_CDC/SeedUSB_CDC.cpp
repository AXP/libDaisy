#include <stdio.h>
#include <string.h>
#include "daisy_seed.h"

static daisy_handle hw;

int main(void)
{
    daisy_seed_init(&hw);
    int  tick_cnt = 0;
    char buff[512];
    while(1)
    {
        dsy_system_delay(500);
        sprintf(buff, "Tick:\t%d\r\n", tick_cnt);
        hw.usb_handle.TransmitInternal((uint8_t*)buff, strlen(buff));
        tick_cnt = (tick_cnt + 1) % 100;
    }
}
