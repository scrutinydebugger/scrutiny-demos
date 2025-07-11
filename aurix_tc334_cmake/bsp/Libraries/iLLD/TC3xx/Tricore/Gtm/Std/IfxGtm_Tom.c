/**
 * \file IfxGtm_Tom.c
 * \brief GTM  basic functionality
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
#pragma warning 508		/* To suppress empty file warning */
#endif
#if defined (__ghs__)
#pragma diag_suppress 96		/* To suppress empty file warning */
#endif
#if !defined(DEVICE_TC33XED) && !defined (DEVICE_TC35X)
#include "IfxGtm_Tom.h"

/******************************************************************************/
/*-------------------------Function Implementations---------------------------*/
/******************************************************************************/

uint32 IfxGtm_Tom_Tgc_buildFeature(uint16 enableMask, uint16 disableMask, uint8 bitfieldOffset)
{
    uint8  i;
    uint32 reg  = 0;
    uint32 mask = enableMask | (disableMask << 16);

    /* Build the register value for the feature enable or disable for multiple channels */
    for (i = 0; i < IFXGTM_TOM_NUM_TGC_CHANNELS; i++)
    {
        /* Bitfield length is 2 bits */
        uint8 shift = (i * 2) + bitfieldOffset;

        if (mask & 0x1)
        {
            reg |= IfxGtm_FeatureControl_enable << shift;
        }

        if (mask & 0x10000)
        {
            reg |= IfxGtm_FeatureControl_disable << shift;
        }

        mask = mask >> 1;
    }

    return reg;
}


uint32 IfxGtm_Tom_Tgc_buildFeatureForChannel(IfxGtm_Tom_Ch channel, boolean enabled, uint8 bitfieldOffset)
{
    uint32 reg = 0;

    /* Bitfield length is 2 bits */
    uint8  shift = ((channel % 8) * 2) + bitfieldOffset;

    /* Build the register value for the feature enable or disable for a single channel */
    if (enabled == 1)
    {
        reg = IfxGtm_FeatureControl_enable << shift;
    }
    else
    {
        reg = IfxGtm_FeatureControl_disable << shift;
    }

    return reg;
}


void IfxGtm_Tom_Tgc_resetChannels(Ifx_GTM_TOM_TGC *tgc, uint32 resetMask)
{
    uint8  i;
    uint32 reg = 0;

    for (i = 0; i < IFXGTM_TOM_NUM_TGC_CHANNELS; i++)
    {
        if (resetMask & 0x1)
        {
            reg |= 1 << i;
        }

        resetMask = resetMask >> 1;
    }

    tgc->GLB_CTRL.U = reg << IFX_GTM_TOM_TGC_GLB_CTRL_RST_CH0_OFF;
}
#endif

#if defined (_TASKING_) || defined (_ghs_)
#pragma restore
#endif
