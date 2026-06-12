#include "scrutiny.hpp"
#include "sci.h"
#include <stdint.h>

static unsigned char rx_buffer[128];
static unsigned char tx_buffer[256];

#pragma DATA_SECTION("ramgs0")
static unsigned char datalogging_buffer[0x2000];

static scrutiny::MainHandler main_handler;
static scrutiny::Config config;
scrutiny::FixedFrequencyLoopHandler task1KHz_LoopHandler(1e7/1000, "Task 1KHz");
static scrutiny::LoopHandler* loops[1] = {
    &task1KHz_LoopHandler
};


static uint32_t SCI_DEVICE = 0;

bool scrutiny_init(uint32_t const sci_base)
{
    SCI_DEVICE = sci_base;
    config.set_buffers(rx_buffer, sizeof(rx_buffer), tx_buffer, sizeof(tx_buffer));
    config.display_name = "TMS320F280049C Demo";
    config.set_datalogging_buffers(datalogging_buffer, sizeof(datalogging_buffer));
    config.set_loops(loops, sizeof(loops)/sizeof(loops[0]));
    return main_handler.init(&config) == scrutiny::Status::SUCCESS;
}

void scrutiny_idle_update(uint32_t const timediff_100ns)
{
    unsigned char byte;

    while (SCI_getRxFIFOStatus(SCI_DEVICE) != SCI_FIFO_RX0)
    {
        byte = SCI_readCharNonBlocking(SCI_DEVICE);
        main_handler.receive_data(&byte, 1);
    }

    main_handler.process(timediff_100ns);

    while (main_handler.data_to_send() > 0 && SCI_getTxFIFOStatus(SCI_DEVICE) != SCI_FIFO_TX16)
    {
         main_handler.pop_data(&byte, 1);
         SCI_writeCharNonBlocking(SCI_DEVICE, byte);
    }

}
