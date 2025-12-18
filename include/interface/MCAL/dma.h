#ifndef DMA_H
#define DMA_H

#include "../../../lib/STD_Types.h"

#define DMA_1   0
#define DMA_2   1

typedef enum{
    DMA_STREAM_0 = 0,
    DMA_STREAM_1,
    DMA_STREAM_2,
    DMA_STREAM_3,
    DMA_STREAM_4,
    DMA_STREAM_5,
    DMA_STREAM_6,
    DMA_STREAM_7
} DMA_Stream_t;

typedef enum{
    DMA_CHANNEL_0 = 0,
    DMA_CHANNEL_1,
    DMA_CHANNEL_2,
    DMA_CHANNEL_3,
    DMA_CHANNEL_4,
    DMA_CHANNEL_5,
    DMA_CHANNEL_6,
    DMA_CHANNEL_7
} DMA_Channel_t;

typedef enum{
    DMA_MEM_BURST_SINGLE = 0,
    DMA_MEM_BURST_INC4,
    DMA_MEM_BURST_INC8,
    DMA_MEM_BURST_INC16
} DMA_MemBurst_t;

typedef enum{
    DMA_PERIPH_BURST_SINGLE = 0,
    DMA_PERIPH_BURST_INC4,
    DMA_PERIPH_BURST_INC8,
    DMA_PERIPH_BURST_INC16
} DMA_PeriphBurst_t;

typedef enum{
    DMA_PRIORITY_LOW = 0,
    DMA_PRIORITY_MEDIUM,
    DMA_PRIORITY_HIGH,
    DMA_PRIORITY_VERY_HIGH
} DMA_Priority_t;

typedef enum{
    DMA_PERIPHERAL_TO_MEMORY = 0,
    DMA_MEMORY_TO_PERIPHERAL,
    DMA_MEMORY_TO_MEMORY
} DMA_Direction_t;

typedef enum{
    DMA_MEM_INC_DISABLE = 0,
    DMA_MEM_INC_ENABLE
} DMA_MemInc_t;

typedef enum{
    DMA_PERIPH_INC_DISABLE = 0,
    DMA_PERIPH_INC_ENABLE
} DMA_PeriphInc_t;

typedef struct{
    uint8_t dmaNum;
    DMA_Stream_t stream;
    DMA_Channel_t channel;
    DMA_MemBurst_t memBurst;
    DMA_PeriphBurst_t periphBurst;
    DMA_MemInc_t memInc;
    DMA_PeriphInc_t periphInc;
    DMA_Priority_t priority;
    DMA_Direction_t direction;
} DMA_Instance_t;

STD_ReturnType DMA_Init(DMA_Instance_t* dmaInstance);
STD_ReturnType DMA_DeInit(DMA_Instance_t* dmaInstance);

STD_ReturnType DMA_Start(DMA_Instance_t* dmaInstance, uint8_t* src, uint8_t* dest, uint8_t size);


#endif // DMA_H