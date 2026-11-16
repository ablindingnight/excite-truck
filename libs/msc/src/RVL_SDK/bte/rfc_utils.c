#include "bt_target.h"
#include "bt_trace.h"
#include "bt_types.h"
#include "btu.h"
#include "data_types.h"
#include "gki.h"
#include "l2c_api.h"
#include "port_api.h"
#include "port_int.h"
#include "rfc_int.h"
#include "rfcdefs.h"
#include <string.h>

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/rfcomm/rfc_utils.c
 */

/******************************************************************************
 *
 *  Copyright (C) 1999-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/* Includes changes by muff1n1634 and for this repository (KoopTheKoopa) */

// DUMP_DATA rfc_crctable u8
static UINT8 const rfc_crctable[256] =
{
#include "dumpdata/rfc_crctable.inc"
};

UINT8 rfc_calc_fcs(UINT16 len, UINT8 *p)
{
	UINT8 fcs = 0xff;

	while (len--)
		fcs = rfc_crctable[fcs ^ *p++];

	return 0xff - fcs;
}

BOOLEAN rfc_check_fcs(UINT16 len, UINT8 *p, UINT8 received_fcs)
{
	UINT8 fcs = 0xff;

	while (len--)
		fcs = rfc_crctable[fcs ^ *p++];

	fcs = rfc_crctable[fcs ^ received_fcs];

	return fcs == 0xcf;
}

tRFC_MCB *rfc_alloc_multiplexer_channel(BD_ADDR bd_addr, BOOLEAN is_initiator)
{
	int i;
	int j;
	tRFC_MCB *p_mcb = NULL;

	for (i = 0; i < MAX_BD_CONNECTIONS; ++i)
	{
		if (rfc_cb.port.rfc_mcb[i].state != RFC_MX_STATE_IDLE
		    && memcmp(rfc_cb.port.rfc_mcb[i].bd_addr, bd_addr, BD_ADDR_LEN) == 0)
		{
			rfc_timer_stop(&rfc_cb.port.rfc_mcb[i]);

			return &rfc_cb.port.rfc_mcb[i];
		}
	}

	/* explicitly post-increment */
	for (i = 0, j = rfc_cb.rfc.last_mux + 1; i < MAX_BD_CONNECTIONS; i++, j++)
	{
		if (j >= MAX_BD_CONNECTIONS)
			j = 0;

		p_mcb = &rfc_cb.port.rfc_mcb[j];

		if (rfc_cb.port.rfc_mcb[j].state == RFC_MX_STATE_IDLE)
		{
			memset(p_mcb, 0, sizeof *p_mcb);
			memcpy(p_mcb->bd_addr, bd_addr, BD_ADDR_LEN);

			GKI_init_q(&p_mcb->cmd_q);

			p_mcb->is_initiator = is_initiator;

			rfc_timer_start(p_mcb, RFC_MCB_INIT_INACT_TIMER);

			rfc_cb.rfc.last_mux = j;
			return p_mcb;
		}
	}

	return NULL;
}

void rfc_release_multiplexer_channel(tRFC_MCB *p_mcb)
{
	void *p_buf;

	rfc_timer_stop(p_mcb);

	while ((p_buf = GKI_dequeue(&p_mcb->cmd_q)) != NULL)
		GKI_freebuf(p_buf);

	memset(p_mcb, 0, sizeof *p_mcb);
	p_mcb->state = RFC_MX_STATE_IDLE;
}

void rfc_timer_start(tRFC_MCB *p_mcb, UINT16 timeout)
{
	TIMER_LIST_ENT *p_tle = &p_mcb->tle;

	RFCOMM_TRACE(EVENT, "rfc_timer_start - timeout:%d", timeout);

	p_tle->param = (TIMER_PARAM_TYPE)p_mcb;

	btu_start_timer(p_tle, BTU_TTYPE_RFCOMM_MFC, timeout);
}

void rfc_timer_stop(tRFC_MCB *p_mcb)
{
	RFCOMM_TRACE(EVENT, "rfc_timer_stop");

	btu_stop_timer(&p_mcb->tle);
}

void rfc_port_timer_start(tPORT *p_port, UINT16 timeout)
{
	TIMER_LIST_ENT *p_tle = &p_port->rfc.tle;

	RFCOMM_TRACE(EVENT, "rfc_port_timer_start - timeout:%d", timeout);

	p_tle->param = (TIMER_PARAM_TYPE)p_port;

	btu_start_timer(p_tle, BTU_TTYPE_RFCOMM_PORT, timeout);
}

void rfc_port_timer_stop(tPORT *p_port)
{
	RFCOMM_TRACE(EVENT, "rfc_port_timer_stop");

	btu_stop_timer(&p_port->rfc.tle);
}

void rfc_check_mcb_active(tRFC_MCB *p_mcb)
{
	UINT16 i;

	for (i = 0; i < RFCOMM_MAX_DLCI; ++i)
	{
		if (p_mcb->port_inx[i])
		{
			p_mcb->is_disc_initiator = FALSE;
			return;
		}
	}

	if (p_mcb->is_disc_initiator)
	{
		p_mcb->is_disc_initiator = FALSE;
		rfc_mx_sm_execute(p_mcb, RFC_MX_EVENT_CLOSE_REQ, NULL);
	}
	else
	{
		rfc_timer_start(p_mcb, RFC_MCB_RELEASE_INACT_TIMER);
	}
}

void rfcomm_process_timeout(TIMER_LIST_ENT *p_tle)
{
	switch (p_tle->event)
	{
	case BTU_TTYPE_RFCOMM_MFC:
		rfc_mx_sm_execute((tRFC_MCB *)p_tle->param, RFC_EVENT_TIMEOUT, NULL);
		break;

	case BTU_TTYPE_RFCOMM_PORT:
		rfc_port_sm_execute((tPORT *)p_tle->param, RFC_EVENT_TIMEOUT, NULL);
		break;

	default:
		break;
	}
}

void rfc_sec_check_complete(BD_ADDR bd_addr, void *p_ref_data, UINT8 res)
{
	tPORT *p_port = p_ref_data;

	if (!p_port->in_use
	    || (p_port->rfc.state != RFC_STATE_ORIG_WAIT_SEC_CHECK
	        && p_port->rfc.state != RFC_STATE_TERM_WAIT_SEC_CHECK))
	{
		return;
	}

	rfc_port_sm_execute(p_ref_data, RFC_EVENT_SEC_COMPLETE, &res);
}

void rfc_port_closed(tPORT *p_port)
{
	tRFC_MCB *p_mcb = p_port->rfc.p_mcb;

	RFCOMM_TRACE(DEBUG, "rfc_port_closed");

	rfc_port_timer_stop(p_port);

	p_port->rfc.state = RFC_STATE_CLOSED;

	if (p_mcb)
	{
		p_mcb->port_inx[p_port->dlci] = 0;
		p_port->dlci = 0;

		rfc_check_mcb_active(p_mcb);
	}

	port_rfc_closed(p_port, PORT_CLOSED);
}

void rfc_inc_credit(tPORT *p_port, UINT8 credit)
{
	if (p_port->rfc.p_mcb->flow == PORT_FC_CREDIT)
	{
		p_port->credit_tx += credit;

		RFCOMM_TRACE(EVENT, "rfc_inc_credit:%d", p_port->credit_tx);

		if (p_port->tx.peer_fc == TRUE)
			PORT_FlowInd(p_port->rfc.p_mcb, p_port->dlci, TRUE);
	}
}

void rfc_dec_credit(tPORT *p_port)
{
	if (p_port->rfc.p_mcb->flow == PORT_FC_CREDIT)
	{
		if (p_port->credit_tx > 0)
			--p_port->credit_tx;

		if (p_port->credit_tx == 0)
			p_port->tx.peer_fc = TRUE;
	}
}

void rfc_check_send_cmd(tRFC_MCB *p_mcb, BT_HDR *p_buf)
{
	BT_HDR *p;

	if (p_buf)
		GKI_enqueue(&p_mcb->cmd_q, p_buf);

	while (!p_mcb->l2cap_congested)
	{
		if ((p = GKI_dequeue(&p_mcb->cmd_q)) == NULL)
			break;

		L2CA_DataWrite(p_mcb->lcid, p);
	}
}
