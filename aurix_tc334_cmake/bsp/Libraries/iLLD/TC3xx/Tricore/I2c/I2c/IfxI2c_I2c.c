/**
 * \file IfxI2c_I2c.c
 * \brief I2C I2C details
 *
 * \version iLLD_1_20_0
 * \copyright Copyright (c) 2024 Infineon Technologies AG. All rights reserved.
 *
 *
 *
 *                                 IMPORTANT NOTICE
 *
 * Use of this file is subject to the terms of use agreed between (i) you or
 * the company in which ordinary course of business you are acting and (ii)
 * Infineon Technologies AG or its licensees. If and as long as no such terms
 * of use are agreed, use of this file is subject to following:
 *
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer, must
 * be included in all copies of the Software, in whole or in part, and all
 * derivative works of the Software, unless such copies or derivative works are
 * solely in the form of machine-executable object code generated by a source
 * language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 *
 */

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

#include "Ifx_Cfg.h"
#if defined (__TASKING__)
#pragma warning 508				/* To suppress empty file warning */
#endif
#if defined (__ghs__)
#pragma diag_suppress 96		/* To suppress empty file warning */
#endif
#if !defined(DEVICE_TC33XED) && !defined(DEVICE_TC33X) 
#include "IfxI2c_I2c.h"

/******************************************************************************/
/*-------------------------Function Implementations---------------------------*/
/******************************************************************************/

float32 IfxI2c_I2c_getBaudrate(IfxI2c_I2c *i2c)
{
    return IfxI2c_getBaudrate(i2c->i2c);
}


void IfxI2c_I2c_initConfig(IfxI2c_I2c_Config *config, Ifx_I2C *i2c)
{
    config->i2c                                            = i2c;
    config->baudrate                                       = 400000;
    config->pins                                           = NULL_PTR;
    config->mode                                           = IfxI2c_Mode_StandardAndFast;

    config->peripheralMode                                 = IfxI2c_MasterNotSlave_master;

    config->addrFifoCfg.addressConfig.slaveAddress         = 0;
    config->addrFifoCfg.addressConfig.addressMode          = IfxI2c_AddressMode_7Bit;
    config->addrFifoCfg.addressConfig.generalCallEnable    = 0;
    config->addrFifoCfg.addressConfig.masterCodeEnable     = 0;
    config->addrFifoCfg.addressConfig.stopOnNotAcknowledge = 0;
    config->addrFifoCfg.addressConfig.stopOnPacketEnd      = 0;

    config->addrFifoCfg.fifoConfig.rxBurstSize             = IfxI2c_RxBurstSize_1Word;
    config->addrFifoCfg.fifoConfig.txBurstSize             = IfxI2c_TxBurstSize_1Word;
    config->addrFifoCfg.fifoConfig.rxFifoAlignment         = IfxI2c_RxFifoAlignment_byte;
    config->addrFifoCfg.fifoConfig.txFifoAlignment         = IfxI2c_TxFifoAlignment_byte;
    config->addrFifoCfg.fifoConfig.rxFifoFlowControl       = IfxI2c_RxFifoFlowControl_enable;
    config->addrFifoCfg.fifoConfig.txFifoFlowControl       = IfxI2c_TxFifoFlowControl_enable;
    config->addrFifoCfg.fifoConfig.clearRequestBehavior    = 0;

    config->timingCfg                          		       = NULL_PTR;
}


void IfxI2c_I2c_initDevice(IfxI2c_I2c_Device *i2cDevice, const IfxI2c_I2c_deviceConfig *i2cDeviceConfig)
{
    i2cDevice->i2c                 = i2cDeviceConfig->i2c;
    i2cDevice->deviceAddress       = i2cDeviceConfig->deviceAddress;
    i2cDevice->addressMode         = i2cDeviceConfig->addressMode;
    i2cDevice->speedMode           = i2cDeviceConfig->speedMode;
    i2cDevice->enableRepeatedStart = i2cDeviceConfig->enableRepeatedStart;
}


void IfxI2c_I2c_initDeviceConfig(IfxI2c_I2c_deviceConfig *i2cDeviceConfig, IfxI2c_I2c *i2c)
{
    i2cDeviceConfig->i2c                 = i2c;
    i2cDeviceConfig->deviceAddress       = 0xff;
    i2cDeviceConfig->addressMode         = IfxI2c_AddressMode_7Bit;
    i2cDeviceConfig->speedMode           = IfxI2c_Mode_StandardAndFast;
    i2cDeviceConfig->enableRepeatedStart = TRUE;
}


void IfxI2c_I2c_initModule(IfxI2c_I2c *i2c, const IfxI2c_I2c_Config *config)
{
    IfxI2c_Config addressFifoConfiglocal = config->addrFifoCfg;
    Ifx_I2C      *i2cSFR                 = config->i2c;
    i2c->i2c = i2cSFR;

    IfxI2c_enableModule(i2cSFR);
    IfxI2c_stop(i2cSFR);                                                                   // enter config Mode
    IfxI2c_configureAsMaster(i2cSFR);

    if (config->peripheralMode == IfxI2c_MasterNotSlave_slave)
    {
        IfxI2c_configureAsSlave(i2cSFR);
    }

    if (config->mode == IfxI2c_Mode_HighSpeed)
    {
        addressFifoConfiglocal.addressConfig.masterCodeEnable = TRUE;
    }

    IfxI2c_configureAddrFifo(i2cSFR, &addressFifoConfiglocal);

    IfxI2c_setBaudrate(i2cSFR, config->baudrate);  /*In high speed mode the baud rate must be higher then 400kHz */

    if (config->timingCfg != NULL_PTR)
	{
		IfxI2c_configureTiming(i2cSFR, config->timingCfg);
	}

    if (config->pins != NULL_PTR)
    {
        IfxI2c_initSclSdaPin(config->pins->scl, config->pins->sda, config->pins->padDriver);
    }

    /* enable the interrupt sources */
    IfxI2c_enableInterruptSources(i2cSFR);
    IfxI2c_run(i2cSFR);

    i2c->baudrate  = IfxI2c_getBaudrate(i2cSFR);
    i2c->busStatus = IfxI2c_getBusStatus(i2cSFR);
    i2c->status    = IfxI2c_I2c_Status_ok;
}


void IfxI2c_enableInterruptSources(Ifx_I2C *i2cSFR)
{
    /* to see protocol interrupt in register RIS enable the corresponding sources */
    /* enable Interrupt Source for arbitrationLost */
    i2cSFR->PIRQSM.U |= (1 << IfxI2c_ProtocolInterruptSource_arbitrationLost);
    /* enable Interrupt Source for notAcknowledgeReceived */
    i2cSFR->PIRQSM.U |= (1 << IfxI2c_ProtocolInterruptSource_notAcknowledgeReceived);
    /* enable Interrupt Source for transmissionEnd */
    i2cSFR->PIRQSM.U |= (1 << IfxI2c_ProtocolInterruptSource_transmissionEnd);
    /* enable Interrupt Source for receiveMode */
    i2cSFR->PIRQSM.U |= (1 << IfxI2c_ProtocolInterruptSource_receiveMode);
    /* to see error interrupt in register RIS enable the corresponding sources */
    /* enable Interrupt Source for rxFifoUnderflow */
    i2cSFR->ERRIRQSM.U |= (1 << IfxI2c_ErrorInterruptSource_rxFifoUnderflow);
    /* enable Interrupt Source for rxFifoOverflow */
    i2cSFR->ERRIRQSM.U |= (1 << IfxI2c_ErrorInterruptSource_rxFifoOverflow);
    /* enable Interrupt Source for txFifoUnderflow */
    i2cSFR->ERRIRQSM.U |= (1 << IfxI2c_ErrorInterruptSource_txFifoUnderflow);
    /* enable Interrupt Source for txFifoOverflow */
    i2cSFR->ERRIRQSM.U |= (1 << IfxI2c_ErrorInterruptSource_txFifoOverflow);
}


IfxI2c_I2c_Status IfxI2c_I2c_read2(IfxI2c_I2c_Device *i2cDevice, volatile uint8 *data, Ifx_SizeT size)
{
    Ifx_I2C          *i2c;                    /* pointer to I2C module */
    IfxI2c_I2c_Status status;                 /* status of I2C and driver */
    IfxI2c_BusStatus  busStatus;              /* status of I2C bus */
    uint32            uiWordsInFifo;          /* value how many words in FIFO available (1. 2 or 4 words) */
    uint32            uiDataIncrement;        /* value how to increment data in FIFO (byte, halfword or word) */
    uint32            uiCnt1, uiCnt2;         /* counters */
    union data
    {
        uint32 packet;
        uint8  packetbyte[4];
    } uiRxData;                               /* data array for transfer from FIFO */

    /* check the size parameter */
    if (size < 0)
    {
        return IfxI2c_I2c_Status_error;
    }

    /* check that the size parameter is not too big for MRPS */
    if (size > 16383)
    {
        return IfxI2c_I2c_Status_error;
    }

    /* set pointer to i2c module */
    i2c             = i2cDevice->i2c->i2c;
    /* set uiWordsInFifo dependent of setting */
    uiWordsInFifo   = 0x1 << i2c->FIFOCFG.B.RXBS;
    /* set uiDataIncrement dependent of setting */
    uiDataIncrement = 0x1 << i2c->FIFOCFG.B.RXFA;
    /* set the actual status to OK */
    status          = IfxI2c_I2c_Status_ok;

    /* get the actual bus status */
    busStatus = IfxI2c_getBusStatus(i2c);

    if ((busStatus != IfxI2c_BusStatus_idle) && (busStatus != IfxI2c_BusStatus_busyMaster))
    {
        /* bus is not free or not own by us, go back to caller */
        status                    = IfxI2c_I2c_Status_busNotFree;
        i2cDevice->i2c->busStatus = busStatus;
        i2cDevice->i2c->status    = status;
        return status;
    }

    if (i2cDevice->addressMode == IfxI2c_AddressMode_10Bit)
    {
        /* for 10 bit addressing we send out the address without any data */
        /* this is only possible when SOPE=0 or in high-speed mode because restart needed */
        /* if high-speed is needed then the function will do this for us */
        status = IfxI2c_I2c_write2(i2cDevice, (uint8 *)0, 0);

        if (status != IfxI2c_I2c_Status_ok)
        {
            return status;
        }

        /* write the first device address byte again */
    }
    else
    {
        /* here we are in state LISTENING or MASTER RESTART */
        /* if we are in LISTENING then maybe switch to high-speed mode if requested */
        if ((i2cDevice->speedMode == IfxI2c_Mode_HighSpeed) && (busStatus == IfxI2c_BusStatus_idle))
        {
            /* switch to high speed mode */
            status = IfxI2c_I2c_switch_to_highspeed(i2c);

            if (status != IfxI2c_I2c_Status_ok)
            {
                /* switch to high speed was not possible because of arbitration lost, FIFO error or master code was acknowledged */
                i2cDevice->i2c->busStatus = IfxI2c_getBusStatus(i2c);
                i2cDevice->i2c->status    = status;
                return status;
            }

            /* write the 7 bit device address, transition M9 */
        }

        /* write the 7 bit device address */
    }

    /* write the first device address byte again or */
    /* write the 7 bit device address, transition M9 or */
    /* write the 7 bit device address */
    IfxI2c_setTransmitPacketSize(i2c, 1);
    /* set the size of bytes which should be read */
    IfxI2c_setReceivePacketSize(i2c, size);

    /* wait until any interrupt flag is set */
    while ((i2c->RIS.U) == 0)
    {}

    if (IfxI2c_isFifoRequest(i2c) == TRUE)
    {
        /* write address to FIFO dependent of the addressing mode with RnW=1 */
        if (i2cDevice->addressMode == IfxI2c_AddressMode_10Bit)
        {
            IfxI2c_writeFifo(i2c, 0xF1 | ((i2cDevice->deviceAddress & 0x30) >> 7));
        }
        else
        {
            IfxI2c_writeFifo(i2c, i2cDevice->deviceAddress | 0x1);
        }

        /* clear the request flag after FIFO write */
        IfxI2c_clearAllDtrInterruptSources(i2c);
    }

    /* wait until any interrupt flag is set */
    while ((i2c->RIS.U) == 0)
    {}

    /* check if we lost arbitration */
    if (IfxI2c_getProtocolInterruptSourceStatus(i2c, IfxI2c_ProtocolInterruptSource_arbitrationLost) == TRUE)
    {
        /* we have lost arbitration, jump back to caller */
        /* clear the arbitration lost interrupt bit */
        IfxI2c_clearProtocolInterruptSource(i2c, IfxI2c_ProtocolInterruptSource_arbitrationLost);
        /* go to state LISTENING via transition M5 */
        /* return information that we lost arbitration */
        status                    = IfxI2c_I2c_Status_al;
        i2cDevice->i2c->busStatus = IfxI2c_getBusStatus(i2c);
        i2cDevice->i2c->status    = status;
        return status;
    }

    /* check if there is an unexpected TX_OFL error */
    if (IfxI2c_getErrorInterruptSourceStatus(i2c, IfxI2c_ErrorInterruptSource_txFifoOverflow) == TRUE)
    {
        /* we have a FIFO overflow, jump back to caller */
        /* clear the FIFO overflow interrupt bit */
        IfxI2c_clearErrorInterruptSource(i2c, IfxI2c_ErrorInterruptSource_txFifoOverflow);

        /* go to state LISTENING via transition M6 */
        /* wait for TX_END is set */
        while (IfxI2c_getProtocolInterruptSourceStatus(i2c, IfxI2c_ProtocolInterruptSource_transmissionEnd) == FALSE)
        {}

        /* TX_END is set and need to be cleared */
        IfxI2c_clearProtocolInterruptSource(i2c, IfxI2c_ProtocolInterruptSource_transmissionEnd);
        /* return that we have an error */
        status                    = IfxI2c_I2c_Status_error;
        i2cDevice->i2c->busStatus = IfxI2c_getBusStatus(i2c);
        i2cDevice->i2c->status    = status;
        return status;
    }

    /* check if there is an unexpected NACK */
    if (IfxI2c_getProtocolInterruptSourceStatus(i2c, IfxI2c_ProtocolInterruptSource_notAcknowledgeReceived) == TRUE)
    {
        /* we have NACK received, jump back to caller */
        /* clear the NACK interrupt bit */
        IfxI2c_clearProtocolInterruptSource(i2c, IfxI2c_ProtocolInterruptSource_notAcknowledgeReceived);

        /* go to state LISTENING via M13 if SONA=1 and not high-speed */
        /* go to state MASTER RESTART via M14 if SONA=0 or high-speed */
        /* wait for TX_END is set */
        while (IfxI2c_getProtocolInterruptSourceStatus(i2c, IfxI2c_ProtocolInterruptSource_transmissionEnd) == FALSE)
        {}

        /* TX_END is set and need to be cleared */
        IfxI2c_clearProtocolInterruptSource(i2c, IfxI2c_ProtocolInterruptSource_transmissionEnd);
        /* return that we have NACK received */
        status                    = IfxI2c_I2c_Status_nak;
        i2cDevice->i2c->busStatus = IfxI2c_getBusStatus(i2c);
        i2cDevice->i2c->status    = status;
        return status;
    }

    /* check that RX is set */
    if (IfxI2c_getProtocolInterruptSourceStatus(i2c, IfxI2c_ProtocolInterruptSource_receiveMode) == FALSE)
    {
        /* RX is not set, jump back to caller */
        /* clear all protocol interrupt bits  */
        IfxI2c_clearAllProtocolInterruptSources(i2c);
        /* return that we have an error */
        status                    = IfxI2c_I2c_Status_error;
        i2cDevice->i2c->busStatus = IfxI2c_getBusStatus(i2c);
        i2cDevice->i2c->status    = status;
        return status;
    }

    /* clear the RX flag */
    IfxI2c_clearProtocolInterruptSource(i2c, IfxI2c_ProtocolInterruptSource_receiveMode);

    while (size)
    {
        /* wait until any interrupt flag is set */
        while ((i2c->RIS.U) == 0)
        {}

        if (IfxI2c_isFifoRequest(i2c) == TRUE)
        {
            /* a request is set, now read from FIFO */
            if ((IfxI2c_getDtrinterruptSourceStatus(i2c, IfxI2c_DtrInterruptSource_burstRequest) == TRUE) ||
                (IfxI2c_getDtrinterruptSourceStatus(i2c, IfxI2c_DtrInterruptSource_lastBurstRequest) == TRUE))
            {
                /* this is a burst request, we make a burst write to FIFO dependent on burst length */
                for (uiCnt1 = 0; uiCnt1 < uiWordsInFifo; uiCnt1++)
                {
                    uiRxData.packet = i2c->RXD.U;

                    /* max. 4 bytes in the word, increment dependent of RX FIFO alignment */
                    for (uiCnt2 = 0; uiCnt2 < 4; uiCnt2 += uiDataIncrement)
                    {
                        if (size)
                        {
                            *data++ = uiRxData.packetbyte[uiCnt2];
                            size--;
                        }
                    }
                }
            }
            else
            {
                /* this is a single request, we make 1 read from FIFO */
                uiRxData.packet = i2c->RXD.U;

                /* max. 4 bytes in the word, increment dependent of RX FIFO alignment */
                for (uiCnt2 = 0; uiCnt2 < 4; uiCnt2 += uiDataIncrement)
                {
                    if (size)
                    {
                        *data++ = uiRxData.packetbyte[uiCnt2];
                        size--;
                    }
                }
            }

            /* clear the request flag after FIFO write */
            IfxI2c_clearAllDtrInterruptSources(i2c);
        }
        else
        {
            /* we have an interrupt but it is not a FIFO request, there is an error */
            /* we jump out of the while loop */
            size = 0;
        }
    }

    /* wait until any interrupt flag is set */
    /* if all bytes are read from FIFO we must wait until an interrupt occurs, otherwise it is already set */
    while ((i2c->RIS.U) == 0)
    {}

    /* check if there is an unexpected RX_OFL error */
    if (IfxI2c_getErrorInterruptSourceStatus(i2c, IfxI2c_ErrorInterruptSource_rxFifoOverflow) == TRUE)
    {
        /* we have a FIFO overflow, jump back to caller */
        /* clear the FIFO overflow interrupt bit */
        IfxI2c_clearErrorInterruptSource(i2c, IfxI2c_ErrorInterruptSource_rxFifoOverflow);

        /* we get all outstanding data */
        while (IfxI2c_isFifoRequest(i2c) == TRUE)
        {
            /* a request is set, now read from FIFO */
            if ((IfxI2c_getDtrinterruptSourceStatus(i2c, IfxI2c_DtrInterruptSource_burstRequest) == TRUE) ||
                (IfxI2c_getDtrinterruptSourceStatus(i2c, IfxI2c_DtrInterruptSource_lastBurstRequest) == TRUE))
            {
                /* this is a burst request, we make a burst write to FIFO dependent on burst length */
                for (uiCnt1 = 0; uiCnt1 < uiWordsInFifo; uiCnt1++)
                {
                    uiRxData.packet = i2c->RXD.U;

                    /* max. 4 bytes in the word, increment dependent of RX FIFO alignment */
                    for (uiCnt2 = 0; uiCnt2 < 4; uiCnt2 += uiDataIncrement)
                    {
                        if (size)
                        {
                            *data++ = uiRxData.packetbyte[uiCnt2];
                            size--;
                        }
                    }
                }
            }
            else
            {
                /* this is a single request, we make 1 read from FIFO */
                uiRxData.packet = i2c->RXD.U;

                /* max. 4 bytes in the word, increment dependent of RX FIFO alignment */
                for (uiCnt2 = 0; uiCnt2 < 4; uiCnt2 += uiDataIncrement)
                {
                    if (size)
                    {
                        *data++ = uiRxData.packetbyte[uiCnt2];
                        size--;
                    }
                }
            }

            /* clear the request flag after FIFO write */
            IfxI2c_clearAllDtrInterruptSources(i2c);
        }

        /* go to state LISTENING via transition M7 */
        /* return that we have an error */
        status = IfxI2c_I2c_Status_error;
        /* wait for TX_END */
    }

    if ((i2cDevice->speedMode == IfxI2c_Mode_HighSpeed) && (IfxI2c_getProtocolInterruptSourceStatus(i2c, IfxI2c_ProtocolInterruptSource_masterCode) == TRUE))
    {
        /* the address was setting the MC bit, clear it */
        IfxI2c_clearProtocolInterruptSource(i2c, IfxI2c_ProtocolInterruptSource_masterCode);
    }

    /* we wait until TX_END is set */
    /* go to state LISTENING via transition M20 if SOPE=1 and not high-speed */
    /* go to state MASTER RESTART via transition M19 if SOPE=0 or high-speed */
    while (IfxI2c_getProtocolInterruptSourceStatus(i2c, IfxI2c_ProtocolInterruptSource_transmissionEnd) == FALSE)
    {}

    /* TX_END is set and need to be cleared */
    IfxI2c_clearProtocolInterruptSource(i2c, IfxI2c_ProtocolInterruptSource_transmissionEnd);
    /* return that everything is ok, we received all bytes */
    i2cDevice->i2c->busStatus = IfxI2c_getBusStatus(i2c);
    i2cDevice->i2c->status    = status;
    return status;
}


IfxI2c_I2c_Status IfxI2c_I2c_write2(IfxI2c_I2c_Device *i2cDevice, volatile uint8 *data, Ifx_SizeT size)
{
    Ifx_I2C          *i2c;                    /* pointer to I2C module */
    IfxI2c_I2c_Status status;                 /* status of I2C and driver */
    IfxI2c_BusStatus  busStatus;              /* status of I2C bus */
    uint32            uiWordsInFifo;          /* value how many words in FIFO available */
    uint32            uiDataIncrement;        /* value how to increment data in FIFO (byte, halfword or word) */
    uint32            uiAddressBytes2Write;   /* value how many bytes are used for device address */
    uint32            uiCnt1, uiCnt2;         /* counters */
    union data
    {
        uint32 packet;
        uint8  packetbyte[4];
    } uiTxData;                                /* data array for transfer to FIFO */

    /* check the size parameter */
    if (size < 0)
    {
        return IfxI2c_I2c_Status_error;
    }

    /* increase size with the length of our address */
    size++;

    if (i2cDevice->addressMode == IfxI2c_AddressMode_10Bit)
    {
        size++;
    }

    /* check that the size parameter is not too big for TPS */
    if (size > 16383)
    {
        return IfxI2c_I2c_Status_error;
    }

    /* set the number of address bytes to write dependent of address setting */
    if (i2cDevice->addressMode == IfxI2c_AddressMode_10Bit)
    {
        uiAddressBytes2Write = 2;
    }
    else
    {
        uiAddressBytes2Write = 1;
    }

    /* set pointer to i2c module */
    i2c             = i2cDevice->i2c->i2c;
    /* set uiWordsInFifo dependent of setting */
    uiWordsInFifo   = 0x1 << i2c->FIFOCFG.B.TXBS;
    /* set uiDataIncrement dependent of setting */
    uiDataIncrement = 0x1 << i2c->FIFOCFG.B.TXFA;
    /* set the actual status to OK */
    status          = IfxI2c_I2c_Status_ok;

    /* get the actual bus status */
    busStatus = IfxI2c_getBusStatus(i2c);

    if ((busStatus != IfxI2c_BusStatus_idle) && (busStatus != IfxI2c_BusStatus_busyMaster))
    {
        /* bus is not free or not own by us, go back to caller */
        status                    = IfxI2c_I2c_Status_busNotFree;
        i2cDevice->i2c->busStatus = busStatus;
        i2cDevice->i2c->status    = status;
        return status;
    }

    /* here we are in state LISTENING or MASTER RESTART */
    /* if we are in LISTENING then maybe switch to high-speed mode if requested */
    if ((i2cDevice->speedMode == IfxI2c_Mode_HighSpeed) && (busStatus == IfxI2c_BusStatus_idle))
    {
        /* switch to high speed mode */
        status = IfxI2c_I2c_switch_to_highspeed(i2c);

        if (status != IfxI2c_I2c_Status_ok)
        {
            /* switch to high speed was not possible because of arbitration lost, FIFO error or master code was acknowledged */
            i2cDevice->i2c->busStatus = IfxI2c_getBusStatus(i2c);
            i2cDevice->i2c->status    = status;
            return status;
        }
    }

    /* set the number of bytes to writes to bus, transition M9 or */
    /* set the number of bytes to writes to bus, transition M1 if IfxI2c_BusStatus_idle or M15 if IfxI2c_BusStatus_busyMaster */
    IfxI2c_setTransmitPacketSize(i2c, size);

    while (size)
    {
        /* wait until any interrupt flag is set */
        while ((i2c->RIS.U) == 0)
        {}

        if (IfxI2c_isFifoRequest(i2c) == TRUE)
        {
            /* a request is set, now write to FIFO */
            if ((IfxI2c_getDtrinterruptSourceStatus(i2c, IfxI2c_DtrInterruptSource_burstRequest) == TRUE) ||
                (IfxI2c_getDtrinterruptSourceStatus(i2c, IfxI2c_DtrInterruptSource_lastBurstRequest) == TRUE))
            {
                /* this is a burst request, we make a burst write to FIFO dependent on burst length */
                for (uiCnt1 = 0; uiCnt1 < uiWordsInFifo; uiCnt1++)
                {
                    /* max. 4 bytes in the word, increment dependent of TX FIFO alignment */
                    for (uiCnt2 = 0; uiCnt2 < 4; uiCnt2 += uiDataIncrement)
                    {
                        if (uiAddressBytes2Write == 0)
                        {
                            /* address was set, set data now if not finished */
                            if (size)
                            {
                                uiTxData.packetbyte[uiCnt2] = *data++;
                            }
                        }
                        else
                        {
                            if (uiAddressBytes2Write == 2)
                            {
                                /* address has 10 bit, set first part of 10 bit address with RnW=0 */
                                uiTxData.packetbyte[uiCnt2] = 0xF0 | ((i2cDevice->deviceAddress & 0x30) >> 7);
                            }
                            else
                            {
                                /* if address has 10 bit, set second part of 10 bit address otherwise set 7 bit address with RnW=0 */
                                if (i2cDevice->addressMode == IfxI2c_AddressMode_10Bit)
                                {
                                    uiTxData.packetbyte[uiCnt2] = (uint8)i2cDevice->deviceAddress;
                                }
                                else
                                {
                                    uiTxData.packetbyte[uiCnt2] = i2cDevice->deviceAddress & 0xFE;
                                }
                            }

                            uiAddressBytes2Write--;
                        }

                        if (size)
                        {
                            size--;
                        }
                    }

                    IfxI2c_writeFifo(i2c, uiTxData.packet);
                }
            }
            else
            {
                /* this is a single request, we make 1 write to FIFO */
                /* max. 4 bytes in the word, increment dependent of TX FIFO alignment */
                for (uiCnt2 = 0; uiCnt2 < 4; uiCnt2 += uiDataIncrement)
                {
                    if (uiAddressBytes2Write == 0)
                    {
                        /* address was set, set data now if not finished */
                        if (size)
                        {
                            uiTxData.packetbyte[uiCnt2] = *data++;
                        }
                    }
                    else
                    {
                        if (uiAddressBytes2Write == 2)
                        {
                            /* address has 10 bit, set first part of 10 bit address with RnW=0 */
                            uiTxData.packetbyte[uiCnt2] = 0xF0 | ((i2cDevice->deviceAddress & 0x30) >> 7);
                        }
                        else
                        {
                            /* if address has 10 bit, set second part of 10 bit address otherwise set 7 bit address with RnW=0 */
                            if (i2cDevice->addressMode == IfxI2c_AddressMode_10Bit)
                            {
                                uiTxData.packetbyte[uiCnt2] = (uint8)i2cDevice->deviceAddress;
                            }
                            else
                            {
                                uiTxData.packetbyte[uiCnt2] = i2cDevice->deviceAddress & 0xFE;
                            }
                        }

                        uiAddressBytes2Write--;
                    }

                    if (size)
                    {
                        size--;
                    }
                }

                IfxI2c_writeFifo(i2c, uiTxData.packet);
            }

            /* clear the request flag after FIFO write */
            IfxI2c_clearAllDtrInterruptSources(i2c);
        }
        else
        {
            /* we have an interrupt but it is not a FIFO request, there is an error */
            /* we jump out of the while loop */
            size = 0;
        }
    }

    /* wait until any interrupt flag is set */
    /* if all bytes are written to FIFO we must wait until an interrupt occurs, otherwise it is already set */
    while ((i2c->RIS.U) == 0)
    {}

    /* check if we lost arbitration */
    if (IfxI2c_getProtocolInterruptSourceStatus(i2c, IfxI2c_ProtocolInterruptSource_arbitrationLost) == TRUE)
    {
        /* we have lost arbitration, jump back to caller */
        /* clear the arbitration lost interrupt bit */
        IfxI2c_clearProtocolInterruptSource(i2c, IfxI2c_ProtocolInterruptSource_arbitrationLost);
        /* go to state LISTENING via transition M5 */
        /* return information that we lost arbitration */
        status                    = IfxI2c_I2c_Status_al;
        i2cDevice->i2c->busStatus = IfxI2c_getBusStatus(i2c);
        i2cDevice->i2c->status    = status;
        return status;
    }

    /* check if there is an unexpected TX_OFL error */
    if (IfxI2c_getErrorInterruptSourceStatus(i2c, IfxI2c_ErrorInterruptSource_txFifoOverflow) == TRUE)
    {
        /* we have a FIFO overflow, jump back to caller */
        /* clear the FIFO overflow interrupt bit */
        IfxI2c_clearErrorInterruptSource(i2c, IfxI2c_ErrorInterruptSource_txFifoOverflow);
        /* go to state LISTENING via transition M6 */
        /* return that we have an error */
        status = IfxI2c_I2c_Status_error;
        /* wait for TX_END */
    }

    /* check if there is an unexpected NACK */
    if (IfxI2c_getProtocolInterruptSourceStatus(i2c, IfxI2c_ProtocolInterruptSource_notAcknowledgeReceived) == TRUE)
    {
        /* we have NACK received, jump back to caller */
        /* clear the NACK interrupt bit */
        IfxI2c_clearProtocolInterruptSource(i2c, IfxI2c_ProtocolInterruptSource_notAcknowledgeReceived);
        /* go to state LISTENING via transition M13 if SONA=1 and not high-speed */
        /* go to state MASTER RESTART via transition M14 if SONA=0 or high-speed */
        /* return that we have NACK received */
        status = IfxI2c_I2c_Status_nak;
        /* wait for TX_END */
    }

    if ((i2cDevice->speedMode == IfxI2c_Mode_HighSpeed) && (IfxI2c_getProtocolInterruptSourceStatus(i2c, IfxI2c_ProtocolInterruptSource_masterCode) == TRUE))
    {
        /* the address was setting the MC bit, clear it */
        IfxI2c_clearProtocolInterruptSource(i2c, IfxI2c_ProtocolInterruptSource_masterCode);
    }

    /* if no error -> go to state LISTENING via transition M3 if SOPE=1 and not high-speed */
    /* if no error -> go to state MASTER RESTART via transition M17 if SOPE=0 or high-speed */
    while (IfxI2c_getProtocolInterruptSourceStatus(i2c, IfxI2c_ProtocolInterruptSource_transmissionEnd) == FALSE)
    {}

    /* TX_END is set and need to be cleared */
    IfxI2c_clearProtocolInterruptSource(i2c, IfxI2c_ProtocolInterruptSource_transmissionEnd);
    /* return that everything is OK, we send out all bytes */
    i2cDevice->i2c->busStatus = IfxI2c_getBusStatus(i2c);
    i2cDevice->i2c->status    = status;
    return status;
}


IfxI2c_I2c_Status IfxI2c_I2c_switch_to_highspeed(Ifx_I2C *i2c)
{
    /* switch to high-speed mode by sending 1 of 8 master codes */
    /* send 1 byte (MASTER CODE) */
    IfxI2c_setTransmitPacketSize(i2c, 1);

    /* we are now in state M1 */
    /* wait until any interrupt flag is set */
    while ((i2c->RIS.U) == 0)
    {}

    if (IfxI2c_isFifoRequest(i2c) == TRUE)
    {
        /* a request is set, now write to FIFO */
        /* Send the Master code to switch to high speed mode */
        IfxI2c_writeFifo(i2c, IFXI2C_HIGHSPEED_MASTER_CODE);
        /* clear the request flag after FIFO write */
        IfxI2c_clearAllDtrInterruptSources(i2c);
    }

    /* wait until a protocol interrupt occurs, if there is FIFO overflow then this will result also in protocol interrupt */
    while (IfxI2c_getProtocolInterruptFlagStatus(i2c) == FALSE)
    {}

    /* check if we lost arbitration */
    if (IfxI2c_getProtocolInterruptSourceStatus(i2c, IfxI2c_ProtocolInterruptSource_arbitrationLost) == TRUE)
    {
        /* we have lost arbitration, jump back to caller */
        /* clear the arbitration iost interrupt bit */
        IfxI2c_clearProtocolInterruptSource(i2c, IfxI2c_ProtocolInterruptSource_arbitrationLost);
        /* go to state LISTENING via transition M5 */
        /* return information that we lost arbitration */
        return IfxI2c_I2c_Status_al;
    }

    /* check if there is an unexpected TX_OFL error */
    if (IfxI2c_getErrorInterruptSourceStatus(i2c, IfxI2c_ErrorInterruptSource_txFifoOverflow) == TRUE)
    {
        /* we have a FIFO overflow, jump back to caller */
        /* clear the FIFO overflow interrupt bit */
        IfxI2c_clearErrorInterruptSource(i2c, IfxI2c_ErrorInterruptSource_txFifoOverflow);

        /* go to state LISTENING via transition M6 */
        /* we wait until TX_END is set */
        while (IfxI2c_getProtocolInterruptSourceStatus(i2c, IfxI2c_ProtocolInterruptSource_transmissionEnd) == FALSE)
        {}

        /* TX_END need to be cleared */
        IfxI2c_clearProtocolInterruptSource(i2c, IfxI2c_ProtocolInterruptSource_transmissionEnd);
        /* return that we have an error */
        return IfxI2c_I2c_Status_error;
    }

    /* check that there is NACK received otherwise go back to caller with error */
    if (IfxI2c_getProtocolInterruptSourceStatus(i2c, IfxI2c_ProtocolInterruptSource_notAcknowledgeReceived) == FALSE)
    {
        /* the MASTER_CODE should not be responded, must be always shows a NACK */
        /* go to LISTENING */
        IfxI2c_releaseBus(i2c);
        /* clear all protocol interrupt sources */
        IfxI2c_clearAllProtocolInterruptSources(i2c);
        /* return actual bus state and information that we have an error */
        return IfxI2c_I2c_Status_error;
    }

    /* we are now in MASTER RESTART via state M8 */
    /* NACK is set and must be cleared */
    IfxI2c_clearProtocolInterruptSource(i2c, IfxI2c_ProtocolInterruptSource_notAcknowledgeReceived);

    /* we wait until TX_END is set */
    while (IfxI2c_getProtocolInterruptSourceStatus(i2c, IfxI2c_ProtocolInterruptSource_transmissionEnd) == FALSE)
    {}

    /* TX_END need to be cleared */
    IfxI2c_clearProtocolInterruptSource(i2c, IfxI2c_ProtocolInterruptSource_transmissionEnd);
    /* we return with everything OK */
    return IfxI2c_I2c_Status_ok;
}
#endif

#if defined (_TASKING_) || defined (_ghs_)
#pragma restore
#endif
