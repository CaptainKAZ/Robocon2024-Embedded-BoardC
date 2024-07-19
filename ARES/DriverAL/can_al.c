#include "can.h"
#include "initcall.h"
#include "macro.h"
#include "slist.h"
#include "squeue.h"
#include "stm32f407xx.h"
#include "can_al.h"
#include "cmsis_os.h"
#include "stm32f4xx_hal_can.h"
#include <stdint.h>

typedef struct CanHw {
  void    *busDev;
  sList   *hooks;
  sQueue   txSoftFifo;
  CanFrame frameBuf[CONFIG_CAN_TX_SOFT_FIFO_LEN];
} CanHw;

CanHw canHws[CAN_DEV_NUM];

int CanHw_tx(CanFrame *frame) {
  ARES_ASSERT(frame->dir == CAN_DIR_TX, "can frame wrong direction");
  CAN_HandleTypeDef  *hcan      = canHws[frame->bus].busDev;
  uint32_t            txMailbox = 0;
  CAN_TxHeaderTypeDef txHeader  = {.DLC   = frame->len,
                                   .IDE   = frame->type == CAN_FRAME_STD ? CAN_ID_STD : CAN_ID_EXT,
                                   .StdId = frame->type == CAN_FRAME_STD ? frame->id : 0,
                                   .ExtId = frame->type == CAN_FRAME_STD ? 0 : frame->id,
                                   .RTR   = CAN_RTR_DATA,
                                   .TransmitGlobalTime = DISABLE};
  if (0 != HAL_CAN_GetTxMailboxesFreeLevel(hcan)) {
    HAL_CAN_AddTxMessage(hcan, &txHeader, frame->data, &txMailbox);
  } else {
    SQueue_push(&canHws[frame->bus].txSoftFifo, frame);
  }
  return ARES_SUCCESS;
}

static void CanHw_txCplt(void *hcan) {
  int busId = -1;
  if (hcan == canHws[0].busDev)
    busId = 0;
  else if (hcan == canHws[1].busDev)
    busId = 1;
  else
    return;
  CanFrame frame;
  if (SQueue_pop(&canHws[busId].txSoftFifo, &frame) == ARES_SUCCESS) {
    CanHw_tx(&frame);
  }
}

int Can_rx(CanFrame *frame) {
  ARES_ASSERT(frame->dir == CAN_DIR_RX, "can frame wrong direction");
  CanHw *hw = &canHws[frame->bus];
  Slist_foreach(hook, hw->hooks) {
    CanRxHook *h = container_of(hook, CanRxHook, entry);
    if (ARES_SUCCESS == h->hook(frame))
      return ARES_SUCCESS;
  }
  return ARES_NOT_IMPLEMENTED;
}

int Can_registerRxHook(CanRxHook *hook) {
  FOR_ARRAY(hw, canHws) { hw->hooks = SList_pushFront(hw->hooks, &hook->entry); }
  return ARES_SUCCESS;
}

/**
 * @brief Just config a default filter that accepts all messages
 * 
 * @param hcan hal can handle
 */
static void CanHw_configDefaultFilter(CAN_HandleTypeDef *hcan) {
  CAN_FilterTypeDef CAN_FilterConfig;
  CAN_FilterConfig.FilterBank  = hcan->Instance == CAN1 ? 0 : 14;
  CAN_FilterConfig.FilterMode  = CAN_FILTERMODE_IDMASK;
  CAN_FilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;

  CAN_FilterConfig.FilterIdHigh = 0;
  CAN_FilterConfig.FilterIdLow  = 0;

  CAN_FilterConfig.FilterMaskIdHigh     = 0;
  CAN_FilterConfig.FilterMaskIdLow      = 0;
  CAN_FilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;

  CAN_FilterConfig.FilterActivation     = ENABLE;
  CAN_FilterConfig.SlaveStartFilterBank = 14;

  if (HAL_CAN_ConfigFilter(hcan, &CAN_FilterConfig) != HAL_OK) {
    Error_Handler();
  }
}

static int CanHw_init(void) {
  CanHw_configDefaultFilter(&hcan1);
  CanHw_configDefaultFilter(&hcan2);
  if (HAL_CAN_Start(&hcan1) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_TX_MAILBOX_EMPTY) !=
      HAL_OK) {
    Error_Handler();
  }
  if (HAL_CAN_Start(&hcan2) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_TX_MAILBOX_EMPTY) !=
      HAL_OK) {
    Error_Handler();
  }
  canHws[0].busDev = &hcan1;
  canHws[1].busDev = &hcan2;
  SQueue_init(&canHws[0].txSoftFifo, sizeof(CanFrame), canHws[0].frameBuf,
              ARRAY_LEN(canHws[0].frameBuf));
  SQueue_init(&canHws[1].txSoftFifo, sizeof(CanFrame), canHws[1].frameBuf,
              ARRAY_LEN(canHws[1].frameBuf));
  return ARES_SUCCESS;
}
Initcall_registerDevice(CanHw_init);

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
  CanFrame            frame;
  CAN_RxHeaderTypeDef RxMsgHdr;
  HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxMsgHdr, frame.data);
  frame.bus  = hcan->Instance == CAN1 ? INTERNAL_CAN1 : INTERNAL_CAN2;
  frame.id   = RxMsgHdr.IDE == CAN_ID_STD ? RxMsgHdr.StdId : RxMsgHdr.ExtId;
  frame.len  = RxMsgHdr.DLC;
  frame.type = RxMsgHdr.IDE == CAN_ID_STD ? CAN_FRAME_STD : CAN_FRAME_EXT;
  frame.dir  = CAN_DIR_RX;
  Can_rx(&frame);
}

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan) { CanHw_txCplt(hcan); }

void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan) { CanHw_txCplt(hcan); }

void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan) { CanHw_txCplt(hcan); }