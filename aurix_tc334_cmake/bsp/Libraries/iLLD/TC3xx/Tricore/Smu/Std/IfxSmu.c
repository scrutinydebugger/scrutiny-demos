/**
 * \file IfxSmu.c
 * \brief SMU  basic functionality
 *
 * \version iLLD_1_20_0
 * \copyright Copyright (c) 2024 Infineon Technologies AG. All rights reserved.
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
 */

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

#include "IfxSmu.h"

/******************************************************************************/
/*-------------------------Function Implementations---------------------------*/
/******************************************************************************/

boolean IfxSmu_activateFSP(void)
{
    /* Activate FSP by Writing SMU_ActivateFSP(0) in CMD register */
    MODULE_SMU.CMD.U = IfxSmu_Command_activateFSP;

    /* Check if Activate FSP command is successfully accepted */
    IFX_ASSERT(IFX_VERBOSE_LEVEL_ERROR, (MODULE_SMU.STS.B.RES == 0));

    return MODULE_SMU.STS.B.RES;
}


boolean IfxSmu_activateRunState(void)
{
    /* Check if SMU is in START state */
    IFX_ASSERT(IFX_VERBOSE_LEVEL_ERROR, (MODULE_SMU.DBG.B.SSM == IfxSmu_SmuState_start));
    /* Write CMD for RUN */
    MODULE_SMU.CMD.U = IfxSmu_Command_start;

    return MODULE_SMU.STS.B.RES;
}


boolean IfxSmu_clearAlarmStatus(IfxSmu_Alarm alarm)
{
    uint16 passwd     = IfxScuWdt_getSafetyWatchdogPassword();
    uint16 alarmGroup = (uint16)alarm / 32;
    uint8  alarmPos   = (uint16)alarm % 32;

    /* Write SMU_ASCE(0) in CMD register */
    MODULE_SMU.CMD.U = IfxSmu_Command_alarmStatusClear;

    /* disable the write-protection for registers */
    IfxScuWdt_clearSafetyEndinit(passwd);

    /* Write 1 in AG bit to clear alarm */
    MODULE_SMU.AG[alarmGroup].U =
        (uint32)((uint32)0x1U << alarmPos);

    /* Restore back the write-protection for registers */
    IfxScuWdt_setSafetyEndinit(passwd);

    return MODULE_SMU.STS.B.RES;
}


void IfxSmu_configAlarmActionPES(uint8 pesAction)
{
    uint16 passwd;

    {
        passwd = IfxScuWdt_getSafetyWatchdogPassword();
        /* disable the write-protection for registers */
        IfxScuWdt_clearSafetyEndinit(passwd);

        /*
         * Port Emergency Stop: enables control of the Port Emergency Stop (PES) feature
         * independently for each internal action. When an action is triggered and if the
         * corresponding bit (as defined below) is set, the hardware triggers
         * automatically a port emergency stop request. Each bit of PES is allocated to
         * an action as follows:
         * 1H SMU_IGCS0 activates PES
         * 2H SMU_IGCS1 activates PES
         * 4H SMU_IGCS2 activates PES
         * 8H SMU_NMI activates PES
         * 10H SMU_CPU_RESET activates PES
         */
        MODULE_SMU.AGC.B.PES = pesAction;

        /* Restore back the write-protection for registers */
        IfxScuWdt_setSafetyEndinit(passwd);
    }
}


void IfxSmu_enableFaultToRunState(boolean enable)
{
    uint16 passwd;

    {
        passwd = IfxScuWdt_getSafetyWatchdogPassword();
        /* disable the write-protection for registers */
        IfxScuWdt_clearSafetyEndinit(passwd);

        /*Enable FAULT to RUN State Transition in Alarm Global Configuration . */
        MODULE_SMU.AGC.B.EFRST = enable;

        /* Restore back the write-protection for registers */
        IfxScuWdt_setSafetyEndinit(passwd);
    }
}


void IfxSmu_enablePortEmergencyStop(boolean enable)
{
    uint16 passwd;

    {
        passwd = IfxScuWdt_getSafetyWatchdogPassword();
        /* disable the write-protection for registers */
        IfxScuWdt_clearSafetyEndinit(passwd);

        /*Configure FSP (fault Signalling Protocol) Port Emergency Stop. */
        MODULE_SMU.FSP.B.PES = enable;

        /* Restore back the write-protection for registers */
        IfxScuWdt_setSafetyEndinit(passwd);
    }
}


boolean IfxSmu_getAlarmAction(IfxSmu_Alarm alarm, IfxSmu_InternalAlarmAction *intAlarmAction)
{
    uint16  alarmGroup = (uint16)alarm / 32;
    uint8   alarmPos   = (uint16)alarm % 32;
    boolean alarmFSPAction;
    /* Read Alarm configuration from SMU alarm CFx (x=0-2) registers
     * and align them to combine to get 3 bit value
     */
    *intAlarmAction = (IfxSmu_InternalAlarmAction)(MODULE_SMU.AGCF[alarmGroup][0].U |
                                                   MODULE_SMU.AGCF[alarmGroup][1].U << 1 |
                                                   MODULE_SMU.AGCF[alarmGroup][2].U << 2);

    /* Assign the FSP action configured to out parameter */
    alarmFSPAction = (boolean)((MODULE_SMU.AGFSP[alarmGroup].U >> alarmPos) & 0x1);

    return alarmFSPAction;
}


uint32 IfxSmu_getAlarmGroupDebugStatus(uint8 alarmGroup)
{
    uint32 alarmStatus;
    /*
     *   Read the alarm status from SMU_ADx (x: 0..6) register
     */
    alarmStatus = MODULE_SMU.AD[alarmGroup].U;
    return alarmStatus;
}


boolean IfxSmu_getAlarmStatus(IfxSmu_Alarm alarm)
{
    uint16  alarmGroup  = (uint16)alarm / 32;
    uint8   alarmPos    = (uint16)alarm % 32;
    /* Read Alarm status */
    boolean alarmStatus = (boolean)((MODULE_SMU.AG[alarmGroup].U >> alarmPos) & 0x1);

    return alarmStatus;
}


boolean IfxSmu_getRTMissedEvent(uint8 timerNum)
{
    boolean timerMissEvent = 0;

    switch (timerNum)
    {
    case 0:
        timerMissEvent = MODULE_SMU.STS.B.RTME0;
        break;
    case 1:
        timerMissEvent = MODULE_SMU.STS.B.RTME1;
        break;
    default:
        IFX_ASSERT(IFX_VERBOSE_LEVEL_ERROR, FALSE);
        break;
    }

    return timerMissEvent;
}


IfxSmu_SmuState IfxSmu_getSmuState(void)
{
    /* Return SMU state read from DBG register */
    return (IfxSmu_SmuState)(MODULE_SMU.DBG.B.SSM);
}


void IfxSmu_lockConfigRegisters(void)
{
    uint16 passwd;

    /* Check if SMU cfg registers is not locked */
    if (MODULE_SMU.KEYS.B.PERLCK != 0xFFU)
    {
        passwd = IfxScuWdt_getSafetyWatchdogPassword();
        /* disable the write-protection for registers */
        IfxScuWdt_clearSafetyEndinit(passwd);

        /* Write the key reg to lock config registers */
        MODULE_SMU.KEYS.B.PERLCK = 0xFFU;

        /* Restore back the write-protection for registers */
        IfxScuWdt_setSafetyEndinit(passwd);
    }
}


boolean IfxSmu_releaseFSP(void)
{
    /* Check if SMU is in FAULT state and EFRST is enabled*/
    IFX_ASSERT(IFX_VERBOSE_LEVEL_ERROR, (IfxSmu_getSmuState() != IfxSmu_SmuState_fault) || (MODULE_SMU.AGC.B.EFRST != 0));

    /* SMU is in START state or (FAULT state with EFRST enabled) */
    /* Release FSP to Fault Free state */
    MODULE_SMU.CMD.U = IfxSmu_Command_releaseFSP;

    /* Check if Release FSP command is accepted by reading the Status reg
     */
    IFX_ASSERT(IFX_VERBOSE_LEVEL_ERROR, (MODULE_SMU.STS.B.RES == 0));

    return MODULE_SMU.STS.B.RES;
}


void IfxSmu_setAlarmAction(IfxSmu_Alarm alarm, IfxSmu_InternalAlarmAction intAlarmAction)
{
    uint32 alarmGroupCF2;
    uint32 alarmGroupCF1;
    uint32 alarmGroupCF0;
    uint32 alarmGroupCFMask;
    uint16 alarmGroup = (uint16)alarm / 32;
    uint8  alarmPos   = (uint16)alarm % 32;
    uint16 passwd     = IfxScuWdt_getSafetyWatchdogPassword();

    IFX_ASSERT(IFX_VERBOSE_LEVEL_ERROR, (MODULE_SMU.KEYS.B.PERLCK != 0xFFU));

    /* Get the mask for configuration registers to set the perticular
     * bits
     */
    alarmGroupCFMask = ~((uint32)0x1U << alarmPos);

    /* Extract Alarm configuration for 3 CFG registers
     * from input parameter
     */
    alarmGroupCF0 = (intAlarmAction & 0x01) << alarmPos;
    alarmGroupCF1 = ((intAlarmAction >> 1) & 0x01) << alarmPos;
    alarmGroupCF2 = ((intAlarmAction >> 2) & 0x01) << alarmPos;

    /* disable the write-protection for registers */
    IfxScuWdt_clearSafetyEndinit(passwd);

    /*Write Config key to configure the SMU registers. */
    MODULE_SMU.KEYS.U = (uint32)0x000000bc;

    /* Write Alarm configuration into corresponding
     *   SMU alarm CFx (x=0-2) registers
     */
    MODULE_SMU.AGCF[alarmGroup][0].U = (MODULE_SMU.AGCF[alarmGroup][0].U & alarmGroupCFMask) | alarmGroupCF0;
    MODULE_SMU.AGCF[alarmGroup][1].U = (MODULE_SMU.AGCF[alarmGroup][1].U & alarmGroupCFMask) | alarmGroupCF1;
    MODULE_SMU.AGCF[alarmGroup][2].U = (MODULE_SMU.AGCF[alarmGroup][2].U & alarmGroupCFMask) | alarmGroupCF2;
    /* After configuration set temporary lock of SMU configuration */
    MODULE_SMU.KEYS.U                = 0;

    /* Restore back the write-protection for registers */
    IfxScuWdt_setSafetyEndinit(passwd);
}


boolean IfxSmu_setAlarmStatus(IfxSmu_Alarm alarm)
{
    uint16          passwd     = IfxScuWdt_getSafetyWatchdogPassword();
    uint16          alarmGroup = (uint16)alarm / 32;
    uint8           alarmPos   = (uint16)alarm % 32;
    IfxSmu_SmuState smuState;
    boolean         status     = 0U;

    /* Read the SMU state from module DBG register */
    smuState = (IfxSmu_SmuState)(MODULE_SMU.DBG.B.SSM);

    if (smuState == IfxSmu_SmuState_start)
    {
        /* SMU is in START state */
        /* All SMU Groups are allowed to set alarm status */

        /* disable the write-protection for registers */
        IfxScuWdt_clearSafetyEndinit(passwd);

        /* Write 1 in AG bit to set alarm */
        MODULE_SMU.AG[alarmGroup].U =
            ((uint32)0x1U << alarmPos);
        /* Restore back the write-protection for registers */
        IfxScuWdt_setSafetyEndinit(passwd);
    }
    else   /* Alarm is in RUN state or FAULT state */
    {
        /* SW Alarm Group/s are only allowed to set alarm status */
        /* Write SMU_ALARM(ARG), where ARG:alarmPos in CMD register to set the Alarm  */
        Ifx_SMU_CMD cmdSfr;
        cmdSfr.U         = MODULE_SMU.CMD.U;
        cmdSfr.B.CMD     = IfxSmu_Command_triggerAlarm;
        cmdSfr.B.ARG     = alarmPos;
        MODULE_SMU.CMD.U = cmdSfr.U;
        status           = MODULE_SMU.STS.B.RES;
    }

    return status;
}


void IfxSmu_setFspMode(IfxSmu_FspMode mode)
{
    uint16 passwd;
    {
        passwd = IfxScuWdt_getSafetyWatchdogPassword();
        /* disable the write-protection for registers */
        IfxScuWdt_clearSafetyEndinit(passwd);

        /*Write Config key to configure the SMU registers. */
        MODULE_SMU.FSP.B.MODE = mode;

        /* Restore back the write-protection for registers */
        IfxScuWdt_setSafetyEndinit(passwd);
    }
}


boolean IfxSmu_stopRT(uint8 timerNum)
{
    /* Write CMD for Stop Recovery Timer */
    Ifx_SMU_CMD cmdSfr;
    cmdSfr.U         = MODULE_SMU.CMD.U;
    cmdSfr.B.CMD     = IfxSmu_Command_stopRT;
    cmdSfr.B.ARG     = timerNum;
    MODULE_SMU.CMD.U = cmdSfr.U;

    return MODULE_SMU.STS.B.RES;
}


boolean IfxSmu_unlockConfigRegisters(void)
{
    uint16  passwd;
    boolean status = 1U;

    /* Check if SMU cfg registers is not locked */
    if (MODULE_SMU.KEYS.B.PERLCK != 0xFFU)
    {
        passwd = IfxScuWdt_getSafetyWatchdogPassword();
        /* disable the write-protection for registers */
        IfxScuWdt_clearSafetyEndinit(passwd);

        /*Write Config key to configure the SMU registers. */
        MODULE_SMU.KEYS.U = (uint32)0x000000bc;

        /* Restore back the write-protection for registers */
        IfxScuWdt_setSafetyEndinit(passwd);
    }
    else
    {
        /* If SMU config is permanently locked then return false */
        status = 0U;
    }

    return status;
}


void IfxSmu_ConfigureInterruptGeneration(IfxSmu_InterruptGenerationConfiguration config, IfxSmu_InterruptRequest intRequest)
{
    uint16      passwd;
    uint8       shift = 0, value = 0;
    uint32      mask  = 0;
    Ifx_SMU_AGC agc;

    agc.U  = MODULE_SMU.AGC.U;
    shift  = (config << 2);
    value  = (1 << intRequest);
    mask   = ~(7 << shift);

    passwd = IfxScuWdt_getSafetyWatchdogPassword();

    /* disable the write-protection for registers */
    IfxScuWdt_clearSafetyEndinit(passwd);

    /*Write Config key to configure the SMU registers. */
    MODULE_SMU.KEYS.U = (uint32)0x000000bc;

    /* Clear the specific bits */
    agc.U &= mask;

    /* Write into the specific bits */
    agc.U            |= ((value) << (shift));

    MODULE_SMU.AGC.U  = agc.U;

    MODULE_SMU.KEYS.U = 0x0U;

    /* Restore back the write-protection for registers */
    IfxScuWdt_setSafetyEndinit(passwd);
}


void IfxSmu_temporaryLockConfigRegisters(void)
{
    uint16 passwd;

    {
        passwd = IfxScuWdt_getSafetyWatchdogPassword();
        /* disable the write-protection for registers */
        IfxScuWdt_clearSafetyEndinit(passwd);

        /*Write Config key to configure temporary lock of the SMU registers. */
        MODULE_SMU.KEYS.U = 0U;

        /* Restore back the write-protection for registers */
        IfxScuWdt_setSafetyEndinit(passwd);
    }
}
