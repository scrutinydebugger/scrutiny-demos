
# Aurix TC334 (CMake) Demo

This project is a demonstration of the integration of Scrutiny on a Infineon Aurix TC334 microcontroller. The project uses CMake and builds with ``tricore-elf-gcc`` provided with AURIX Development Studio. This demo runs a minimalistic task scheduler with 3 premptive tasks 
 
 - 10 Khz High Priority
 - 1Khz Low Priority
 - Idle Loop

 Each of these tasks run a software wave generator that can generate common periodic waves: sine, triangular, square, sawtooth. These wave generators exist to showcase the graphing capabilities of Scrutiny.

 The device communicate through the USB virtual UART at 115200 Bauds/sec (1 stop bit, no parity)

## Required hardware:

 - [kit_a2g_tc334_lite development board](https://www.infineon.com/cms/en/product/evaluation-boards/kit_a2g_tc334_lite/)
 - USB Cable
 - \[Optional\]: Logic Analyzer (for recreating the waveform provided below)

## Required software:

 - GCC for tricore (available with the download of [AURIX Development Studio](https://www.infineon.com/cms/en/product/promopages/aurix-development-studio/))
 - CMake
 - \[Optional\]: GNU Make
 - \[Optional\]: Bash (Git bash for Widnows is fine)

## Building

Assuming the software requirements are all met and the software are in your PATH, calling the following scripts should be enough for building and flashing.

```bash
./scripts/build.sh
./scripts/flash.sh
```

## Prebuilt binary

The prebuilt binary (ready to be flashed) & the Scrutiny Firmware File (.sfd) to be loaded onto the server are located in ``./prebuilt``

## Pin Mapping

 -  P00.0 ( Header X2 - Pin 3) : Low frequency (1KHz) task
 -  P00.2 ( Header X2 - Pin 5) : High frequency (10KHz) task
 -  P00.1 ( Header X2 - Pin 4) : Time reference. 1Hz square wave to validate the correctness of time emasurement.
 -  P00.8 ( Header X2 - Pin 9) : Datalogger (embedded graph) trigger pulse
 -  AscLin0 : UART conencted to the USB port
 -  AscLin1 (Tx: P15.1): Debug UART. retransmit everything received on AscLin0. 


## Task scheduling

The task scheduling is done using the GPT12 module. 

- GPT12.T2 interrupt scehdule the 10KHz loop. 
- GPT12.T4 schedule the 1KHz loop. 

The 10KHz can interrupt the 1KHz and idle loop. The 1KHz task **cannot** interrupt the 10KHz task, but can interrupt the idle loop.

![Tasks Timing](images/tasks-timing.png)

Both timer driven tasks executes a dummy for-loop with a length that can be controlled via scrutiny. 

```c++
    static volatile uint32_t highfreq_load = 1000; // Can be modified by scrutiny
    for (uint32_t i = 0; i < highfreq_load; i++)
        ;
```

The graph below shows what happens when these changes are made : 

 - ``/static/main.cpp/user_task_lowfreq()/lowfreq_load`` : Changed from 10000 to 50000
 - ``/static/main.cpp/user_task_highfreq()/highfreq_load`` : Changed from 1000 to 5000

![Tasks Timing](images/tasks-timing-loaded.png)

If a load is increased at the point where a new interrupt is fired before the previous has finished executing, an overrun condition is triggered. In that situation, the task is disabled and LED1 is turned on.

