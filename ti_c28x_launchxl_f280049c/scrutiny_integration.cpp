#include "sci.h"
#include "scrutiny.hpp"
#include "board.h"
#include <stdint.h>

static uint32_t SCI_DEVICE = 0; // UART on which to talk. Given by main().

// Communication buffer
static unsigned char rx_buffer[128];
static unsigned char tx_buffer[256];    // We generally benefit from a bigger TX buffer to increase memory dump speed.

#pragma DATA_SECTION("ramgs0")
static unsigned char datalogging_buffer[0x2000]; 

static scrutiny::MainHandler main_handler;
scrutiny::FixedFrequencyLoopHandler task1KHz_LoopHandler(1e7 / 1000, "Task 1KHz");
static scrutiny::LoopHandler *loops[1] = { &task1KHz_LoopHandler };     // Don't put on the stack

// === Below, we showcase the Scrutiny ability to define protected regions. ===
struct SpecialRegionDemoStruct
{
    int int1;
    int int2;
    unsigned int uint3;
    bool bool1;
};

static SpecialRegionDemoStruct my_forbidden_struct = {-10,20,30, true};     // We will never see those values.
static SpecialRegionDemoStruct my_read_only_struct = {-100,200,300, true};  // These won't be writable.

static scrutiny::AddressRange forbidden_regions[] = {   // Don't put on the stack
    scrutiny::tools::make_address_range(&my_forbidden_struct, sizeof(my_forbidden_struct))
};

static scrutiny::AddressRange read_only_regions[] = {   // Don't put on the stack
    scrutiny::tools::make_address_range(&my_read_only_struct, sizeof(my_forbidden_struct))
};
// ======

// ====== Runtime Published Values - We tie a LED to one. No need of debug symbol to find those. =====
bool rpv_write_callback(const scrutiny::RuntimePublishedValue rpv, const scrutiny::AnyType *inval, scrutiny::LoopHandler* caller)
{   
    static_cast<void>(caller);
    if (rpv.id == 0x1000){
        GPIO_writePin(LED5, (inval->boolean) ? 0 : 1);
    }
    else {
        return false;
    }
    return true;
}

bool rpv_read_callback(scrutiny::RuntimePublishedValue rpv, scrutiny::AnyType *outval, scrutiny::LoopHandler* caller)
{   
    static_cast<void>(caller);
    if (rpv.id == 0x1000){
        outval->boolean = (GPIO_readPin(LED5) == 0);
    }
    else {
        return false;
    }
    return true;
}

scrutiny::RuntimePublishedValue rpvs[] = {
    {0x1000, scrutiny::VariableType::boolean}
};

// =====

bool scrutiny_init(uint32_t const sci_base)
{
    SCI_DEVICE = sci_base;
    scrutiny::Config config;    // Fine to put this on the stack.
    config.set_buffers(rx_buffer, sizeof(rx_buffer), tx_buffer, sizeof(tx_buffer));
    config.display_name = "LaunchXL-F280049C Demo"; // Max 32 chars
    config.set_datalogging_buffers(datalogging_buffer, sizeof(datalogging_buffer));
    config.set_loops(loops, sizeof(loops) / sizeof(loops[0]));
    config.set_forbidden_address_range(forbidden_regions, sizeof(forbidden_regions)/sizeof(forbidden_regions[0]));
    config.set_readonly_address_range(read_only_regions, sizeof(read_only_regions)/sizeof(read_only_regions[0]));
    config.set_published_values(rpvs, sizeof(rpvs)/sizeof(rpvs[0]), rpv_read_callback, rpv_write_callback);
    return main_handler.init(&config) == scrutiny::Status::SUCCESS;
}

void scrutiny_idle_update(uint32_t const timediff_100ns)
{
    // TI does not provide an API to know how many bytes are avaialble in the FIFO, so we loop byte per byte.
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
