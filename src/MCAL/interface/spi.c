#include "interface/MCAL/spi.h"
#include "private/MCAL/spi_priv.h"
#include "interface/MCAL/rcc.h"
#include "interface/Core/nvic.h"
#include "interface/MCAL/gpio.h"

// HSPI ISR Handlers
void SPI1_ITHandler(void);
void SPI2_ITHandler(void);
void SPI3_ITHandler(void);
void SPI4_ITHandler(void);

static STD_ReturnType SPI_GPIO_Config(SPI_Number_t spiNum, SPI_Mode_t mode);

static SPI_Instance_t SPI[4] = {SPI1, SPI2, SPI3, SPI4};
static GPIO_t spi1_ss_pin;
static GPIO_t spi2_ss_pin;
static GPIO_t spi3_ss_pin;

volatile SPI_State_t spiState[4];
volatile SPI_Buffer_t* spiBuf[4];

STD_ReturnType SPI_Init(const SPI_Config_t* config, SYSTICK_ClockSource_t clockSource){
    STD_ReturnType ret = STD_SUCCESS;

    if(config == NULL){
        ret = STD_ERROR;
    }
    else{
        // 1. Enable RCC
        switch(config->spiNum){
            case SPI_1:
                ret = RCC_ControlPeripheral(RCC_SPI1, RCC_PERIPHERAL_ENABLE);
                break;
            case SPI_2:
                ret = RCC_ControlPeripheral(RCC_SPI2, RCC_PERIPHERAL_ENABLE);
                break;
            case SPI_3:
                ret = RCC_ControlPeripheral(RCC_SPI3, RCC_PERIPHERAL_ENABLE);
                break;
            case SPI_4:
                ret = RCC_ControlPeripheral(RCC_SPI4, RCC_PERIPHERAL_ENABLE);
                break;
            default:
                return STD_ERROR;
                break;
        }

        // 2. Set Baud Rate Prescaler
        // SPI1 & SPI4 Max is 42MHz - SPI2 & SPI3 Max is 21MHz
        if(clockSource == SYSTICK_CLOCK_SOURCE_PLL_MAX && (config->spiNum == SPI_2 || config->spiNum == SPI_3) && config->baudRatePrescaler == SPI_BAUDRATE_PRESCALER_4){
            SPI[config->spiNum]->CR1 |= (SPI_BAUDRATE_PRESCALER_4 << 3); // Set prescaler to 4
        }
        else{
            SPI[config->spiNum]->CR1 |= ((config->baudRatePrescaler & 0x7) << 3); // Just set prescaler bits
        }

        // 3. Frame Format Standard
        SPI[config->spiNum]->CR2 &= ~((config->frameFormatStandard & 0x1) << 4);

        // 4. Clock Polarity
        SPI[config->spiNum]->CR1 |= ((config->clockPolarity & 0x1) << 1);

        // 5. Clock Phase
        SPI[config->spiNum]->CR1 |= ((config->clockPhase & 0x1) << 0);

        // 6. Data Frame Format
        SPI[config->spiNum]->CR1 |= ((config->dataFrame & 0x1) << 11);

        // 7. Frame Format
        SPI[config->spiNum]->CR1 |= ((config->frameFormat & 0x1) << 7);

        // 8. Configure Master/Slave Mode
        if(config->mode == SPI_MODE_MASTER){
            SPI[config->spiNum]->CR1 |= (1U << 9); // SSM = 1
            SPI[config->spiNum]->CR1 |= (1U << 8); // SSI = 1
            SPI[config->spiNum]->CR1 |= (1U << 2); // MSTR = 1
        }
        else{
            SPI[config->spiNum]->CR1 &= ~(1U << 2); // MSTR = 0
        }

        // 9. Configure GPIO Pins for SPI Functionality
        ret = SPI_GPIO_Config(config->spiNum, config->mode);
        if(ret != STD_SUCCESS){
            return ret;
        }

        // 10. Configure DMA if enabled
        if(config->dmaEnable == SPI_DMA_ENABLE){
            SPI[config->spiNum]->CR2 |= (1U << 1);
            SPI[config->spiNum]->CR2 |= (1U << 0);
        }
        else{
            // Nothing
        }

        // 11. Set Initial State
        spiState[config->spiNum] = SPI_STATE_READY;
        spiBuf[config->spiNum] = NULL;

        // 12. Enable SPI Peripheral
        SPI[config->spiNum]->CR1 |= (1U << 6);
    }

    return ret;
}

STD_ReturnType SPI_DeInit(const SPI_Config_t* config){
    STD_ReturnType ret = STD_SUCCESS;

    if(config == NULL){
        ret = STD_ERROR;
    }
    else{
        // 1. Disable SPI Peripheral
        SPI[config->spiNum]->CR1 &= ~(1U << 6);

        // 2. Reset Control Registers
        SPI[config->spiNum]->CR1 = 0U;
        SPI[config->spiNum]->CR2 = 0U;

        // 3. Reset SPI Peripheral
        switch(config->spiNum){
            case SPI_1:
                ret = RCC_ControlPeripheral(RCC_SPI1, RCC_PERIPHERAL_RESET);
                break;
            case SPI_2:
                ret = RCC_ControlPeripheral(RCC_SPI2, RCC_PERIPHERAL_RESET);
                break;
            case SPI_3:
                ret = RCC_ControlPeripheral(RCC_SPI3, RCC_PERIPHERAL_RESET);
                break;
            case SPI_4:
                ret = RCC_ControlPeripheral(RCC_SPI4, RCC_PERIPHERAL_RESET);
                break;
            default:
                return STD_ERROR;
                break;
        }
    }

    return ret;
}

STD_ReturnType SPI_Tranceive(const SPI_Config_t* config, SPI_Buffer_t* buffer, uint32_t timeout){
    STD_ReturnType ret = STD_SUCCESS;

    if(config == NULL ||  buffer == NULL || buffer->txData == NULL || buffer->rxData == NULL || buffer->length == 0) {
        ret = STD_ERROR;
    }
    else{
        if(config->dataFrame == SPI_DATA_FRAME_8BIT){
            for(uint8_t i = 0; i < buffer->length; i++){
                // Wait until TX buffer is empty
                uint32_t tickStart = 0;
                while(!(SPI[config->spiNum]->SR & (1U << 1))){
                    SYSTICK_DelayMS(1);
                    if(tickStart++ > timeout){
                        return STD_TIMEOUT;
                    }
                }
                // Send data
                *((volatile uint8_t*)&SPI[config->spiNum]->DR) = *(uint8_t*)&(buffer->txData[i]);

                // Wait until RX buffer is not empty
                tickStart = 0;
                while(!(SPI[config->spiNum]->SR & (1U << 0))){
                    SYSTICK_DelayMS(1);
                    if(tickStart++ > timeout){
                        return STD_TIMEOUT;
                    }
                }
                // Receive data
                *(uint8_t*)&(buffer->rxData[i]) = *((volatile uint8_t*)&SPI[config->spiNum]->DR);
                // wait BSY = 0
                while(SPI[config->spiNum]->SR & (1U << 7));
            }
        }
        else if(config->dataFrame == SPI_DATA_FRAME_16BIT){
            for(uint8_t i = 0; i < buffer->length; i += 2){
                // Wait until TX buffer is empty
                uint32_t tickStart = 0;
                while(!(SPI[config->spiNum]->SR & (1U << 1))){
                    SYSTICK_DelayMS(1);
                    if(tickStart++ > timeout){
                        return STD_TIMEOUT;
                    }
                }
                // Send data
                *((volatile uint16_t*)&SPI[config->spiNum]->DR) = *((uint16_t*)&(buffer->txData[i]));

                // Wait until RX buffer is not empty
                tickStart = 0;
                while(!(SPI[config->spiNum]->SR & (1U << 0))){
                    SYSTICK_DelayMS(1);
                    if(tickStart++ > timeout){
                        return STD_TIMEOUT;
                    }
                }
                // Receive data
                *((uint16_t*)&(buffer->rxData[i])) = *((volatile uint16_t*)&SPI[config->spiNum]->DR);
                // wait BSY = 0
                while(SPI[config->spiNum]->SR & (1U << 7));
            }
        }
        else{
            ret = STD_ERROR;
        }
    }

    return ret;
}

STD_ReturnType SPI_TranceiveIT(const SPI_Config_t* config, SPI_Buffer_t* buffer){
    STD_ReturnType ret = STD_SUCCESS;
    
    if(config == NULL ||  buffer == NULL || buffer->txData == NULL || buffer->rxData == NULL || buffer->length == 0) {
        ret = STD_ERROR;
    }
    else{
        if(spiState[config->spiNum] == SPI_STATE_READY){
            spiState[config->spiNum] = SPI_STATE_BUSY;
            // Save Buffer Reference
            spiBuf[config->spiNum] = buffer;
            spiBuf[config->spiNum]->txIndex = 0;
            spiBuf[config->spiNum]->rxIndex = 0;

            // Enable SPI IRQ in NVIC
            ret = SPI_NVIC_IRQ_Enable(config->spiNum);
            if(ret != STD_SUCCESS){
                return ret;
            }

            // Enable TXE and RXNE Interrupts
            SPI[config->spiNum]->CR2 |= (1U << 7); 
            SPI[config->spiNum]->CR2 |= (1U << 6); 
        }
        else if(spiState[config->spiNum] == SPI_STATE_BUSY){
            ret = STD_BUSY;
        }
        else{
            ret = STD_ERROR;
        }
    }

    return ret;
}

STD_ReturnType SPI_GetTXEFlag(SPI_Number_t spiNum, uint8_t* status){
    STD_ReturnType ret = STD_SUCCESS;

    if(status == NULL || spiNum > SPI_4){
        ret = STD_ERROR;
    }
    else{
        *status = (SPI[spiNum]->SR & (1U << 1)) >> 1;
    }

    return ret;
}

STD_ReturnType SPI_GetRXNEFlag(SPI_Number_t spiNum, uint8_t* status){
    STD_ReturnType ret = STD_SUCCESS;

    if(status == NULL || spiNum > SPI_4){
        ret = STD_ERROR;
    }
    else{
        *status = SPI[spiNum]->SR & 1U;
    }

    return ret;
}

STD_ReturnType SPI_GetBusyFlag(SPI_Number_t spiNum, uint8_t* status){
    STD_ReturnType ret = STD_SUCCESS;

    if(status == NULL || spiNum > SPI_4){
        ret = STD_ERROR;
    }
    else{
        *status = (SPI[spiNum]->SR & (1U << 7)) >> 7;
    }

    return ret;
}

STD_ReturnType SPI_GetState(SPI_Number_t spiNum, uint8_t* state){
    STD_ReturnType ret = STD_SUCCESS;

    if(state == NULL || spiNum > SPI_4){
        ret = STD_ERROR;
    }
    else{
        *state = spiState[spiNum];
    }

    return ret;
}

STD_ReturnType SPI_SetState(SPI_Number_t spiNum, uint8_t state){
    STD_ReturnType ret = STD_SUCCESS;

    if(spiNum > SPI_4){
        ret = STD_ERROR;
    }
    else{
        spiState[spiNum] = state;
    }

    return ret;
}

STD_ReturnType SPI_NVIC_IRQ_Enable(SPI_Number_t spiNum){
    STD_ReturnType ret = STD_SUCCESS;

    switch(spiNum){
        case SPI_1:
            ret = NVIC_EnableIRQ(SPI1_IRQn);
            break;
        case SPI_2:
            ret = NVIC_EnableIRQ(SPI2_IRQn);
            break;
        case SPI_3:
            ret = NVIC_EnableIRQ(SPI3_IRQn);
            break;
        case SPI_4:
            ret = NVIC_EnableIRQ(SPI4_IRQn);
            break;
        default:
            ret = STD_ERROR;
            break;
    }

    return ret;
}

STD_ReturnType SPI_NVIC_TXE_RXNE_Disable(SPI_Number_t spiNum){
    STD_ReturnType ret = STD_SUCCESS;

    SPI[spiNum]->CR2 &= ~((1U << 7) | (1U << 6));

    switch(spiNum){
        case SPI_1:
            ret = NVIC_DisableIRQ(SPI1_IRQn);
            break;
        case SPI_2:
            ret = NVIC_DisableIRQ(SPI2_IRQn);
            break;
        case SPI_3:
            ret = NVIC_DisableIRQ(SPI3_IRQn);
            break;
        case SPI_4:
            ret = NVIC_DisableIRQ(SPI4_IRQn);
            break;
        default:
            ret = STD_ERROR;
            break;
    }

    return ret;
}

STD_ReturnType SPI_GetDataSize(SPI_Number_t spiNum, uint8_t* dataSize){
    STD_ReturnType ret = STD_SUCCESS;

    if(spiNum > SPI_4 || dataSize == NULL){
        ret = STD_ERROR;
    }
    else{
        *dataSize = (SPI[spiNum]->CR1 >> 11) & 0x1;
    }

    return ret;
}

STD_ReturnType SPI_GetDR8Bits(SPI_Number_t spiNum, uint8_t* data){
    STD_ReturnType ret = STD_SUCCESS;

    if(spiNum > SPI_4 || data == NULL){
        ret = STD_ERROR;
    }
    else{
        *data = *((volatile uint8_t*)&SPI[spiNum]->DR);
    }

    return ret;
}

STD_ReturnType SPI_SetDR8Bits(SPI_Number_t spiNum, uint8_t* data){
    STD_ReturnType ret = STD_SUCCESS;

    if(spiNum > SPI_4 || data == NULL){
        ret = STD_ERROR;
    }
    else{
        *((volatile uint8_t*)&SPI[spiNum]->DR) = *data;
    }

    return ret;
}

STD_ReturnType SPI_GetDR16Bits(SPI_Number_t spiNum, uint8_t* data){
    STD_ReturnType ret = STD_SUCCESS;

    if(spiNum > SPI_4 || data == NULL){
        ret = STD_ERROR;
    }
    else{
        *((uint16_t*)data) = *((volatile uint16_t*)&SPI[spiNum]->DR);
    }

    return ret;
}

STD_ReturnType SPI_SetDR16Bits(SPI_Number_t spiNum, uint8_t* data){
    STD_ReturnType ret = STD_SUCCESS;

    if(spiNum > SPI_4 || data == NULL){
        ret = STD_ERROR;
    }
    else{
        *((volatile uint16_t*)&SPI[spiNum]->DR) = *((uint16_t*)data);
    }

    return ret;
}

uint32_t SPI_DRAddress(SPI_Number_t spiNum){
    return (uint32_t)&(SPI[spiNum]->DR);
}

void SPI1_IRQHandler(void){
    SPI1_ITHandler();
}

void SPI2_IRQHandler(void){
    SPI2_ITHandler();
}

void SPI3_IRQHandler(void){
    SPI3_ITHandler();
}

void SPI4_IRQHandler(void){
    SPI4_ITHandler();
}

static STD_ReturnType SPI_GPIO_Config(SPI_Number_t spiNum, SPI_Mode_t mode){
    STD_ReturnType ret = STD_SUCCESS;
    GPIO_t spi_mosi;
    GPIO_t spi_miso;
    GPIO_t spi_sck;

    switch(spiNum){
        case SPI_1:
            spi_mosi = (GPIO_t){
                .port = GPIO_PORTA,
                .pin = GPIO_PIN_7,
                .mode = GPIO_MODE_AF,
                .speed = GPIO_SPEED_HIGH,
                .outputType = GPIO_OUTPUT_PUSHPULL,
                .pullType = GPIO_NOPULL,
                .altFunc = GPIO_AF5_SPI1_2
            };
            ret = GPIO_Init(&spi_mosi);
            if(ret != STD_SUCCESS){
                return ret;
            }
            spi_miso = (GPIO_t){
                .port = GPIO_PORTA,
                .pin = GPIO_PIN_6,
                .mode = GPIO_MODE_AF,
                .speed = GPIO_SPEED_HIGH,
                .outputType = GPIO_OUTPUT_PUSHPULL,
                .pullType = GPIO_NOPULL,
                .altFunc = GPIO_AF5_SPI1_2
            };
            ret = GPIO_Init(&spi_miso);
            if(ret != STD_SUCCESS){
                return ret;
            }
            spi_sck = (GPIO_t){
                .port = GPIO_PORTA,
                .pin = GPIO_PIN_5,
                .mode = GPIO_MODE_AF,
                .speed = GPIO_SPEED_HIGH,
                .outputType = GPIO_OUTPUT_PUSHPULL,
                .pullType = GPIO_NOPULL,
                .altFunc = GPIO_AF5_SPI1_2
            };
            ret = GPIO_Init(&spi_sck);
            if(ret != STD_SUCCESS){
                return ret;
            }
            if(mode == SPI_MODE_SLAVE){
                spi1_ss_pin = (GPIO_t){
                    .port = GPIO_PORTA,
                    .pin = GPIO_PIN_4,
                    .mode = GPIO_MODE_AF,
                    .speed = GPIO_SPEED_HIGH,
                    .outputType = GPIO_OUTPUT_PUSHPULL,
                    .pullType = GPIO_NOPULL,
                    .altFunc = GPIO_AF5_SPI1_2
                };
                ret = GPIO_Init(&spi1_ss_pin);
            }
            else{
                // Nothing -> Master mode handles SS pin via software
            }
            break;

        case SPI_2:
            spi_mosi = (GPIO_t){
                .port = GPIO_PORTB,
                .pin = GPIO_PIN_15,
                .mode = GPIO_MODE_AF,
                .speed = GPIO_SPEED_HIGH,
                .outputType = GPIO_OUTPUT_PUSHPULL,
                .pullType = GPIO_NOPULL,
                .altFunc = GPIO_AF5_SPI1_2
            };
            ret = GPIO_Init(&spi_mosi);
            if(ret != STD_SUCCESS){
                return ret;
            }
            spi_miso = (GPIO_t){
                .port = GPIO_PORTB,
                .pin = GPIO_PIN_14,
                .mode = GPIO_MODE_AF,
                .speed = GPIO_SPEED_HIGH,
                .outputType = GPIO_OUTPUT_PUSHPULL,
                .pullType = GPIO_NOPULL,
                .altFunc = GPIO_AF5_SPI1_2
            };
            ret = GPIO_Init(&spi_miso);
            if(ret != STD_SUCCESS){
                return ret;
            }
            spi_sck = (GPIO_t){
                .port = GPIO_PORTB,
                .pin = GPIO_PIN_13,
                .mode = GPIO_MODE_AF,
                .speed = GPIO_SPEED_HIGH,
                .outputType = GPIO_OUTPUT_PUSHPULL,
                .pullType = GPIO_NOPULL,
                .altFunc = GPIO_AF5_SPI1_2
            };
            ret = GPIO_Init(&spi_sck);
            if(ret != STD_SUCCESS){
                return ret;
            }
            if(mode == SPI_MODE_SLAVE){
                spi2_ss_pin = (GPIO_t){
                    .port = GPIO_PORTB,
                    .pin = GPIO_PIN_12,
                    .mode = GPIO_MODE_AF,
                    .speed = GPIO_SPEED_HIGH,
                    .outputType = GPIO_OUTPUT_PUSHPULL,
                    .pullType = GPIO_NOPULL,
                    .altFunc = GPIO_AF5_SPI1_2
                };
                ret = GPIO_Init(&spi2_ss_pin);
            }
            else{
                // Nothing -> Master mode handles SS pin via software
            }
            break;

        case SPI_3:
            spi_mosi = (GPIO_t){
                .port = GPIO_PORTB,
                .pin = GPIO_PIN_5,
                .mode = GPIO_MODE_AF,
                .speed = GPIO_SPEED_HIGH,
                .outputType = GPIO_OUTPUT_PUSHPULL,
                .pullType = GPIO_NOPULL,
                .altFunc = GPIO_AF6_SPI3
            };
            ret = GPIO_Init(&spi_mosi);
            if(ret != STD_SUCCESS){
                return ret;
            }
            spi_miso = (GPIO_t){
                .port = GPIO_PORTB,
                .pin = GPIO_PIN_4,
                .mode = GPIO_MODE_AF,
                .speed = GPIO_SPEED_HIGH,
                .outputType = GPIO_OUTPUT_PUSHPULL,
                .pullType = GPIO_NOPULL,
                .altFunc = GPIO_AF6_SPI3
            };
            ret = GPIO_Init(&spi_miso);
            if(ret != STD_SUCCESS){
                return ret;
            }
            spi_sck = (GPIO_t){
                .port = GPIO_PORTB,
                .pin = GPIO_PIN_3,
                .mode = GPIO_MODE_AF,
                .speed = GPIO_SPEED_HIGH,
                .outputType = GPIO_OUTPUT_PUSHPULL,
                .pullType = GPIO_NOPULL,
                .altFunc = GPIO_AF6_SPI3
            };
            ret = GPIO_Init(&spi_sck);
            if(ret != STD_SUCCESS){
                return ret;
            }
            if(mode == SPI_MODE_SLAVE){
                spi3_ss_pin = (GPIO_t){
                    .port = GPIO_PORTB,
                    .pin = GPIO_PIN_6,
                    .mode = GPIO_MODE_AF,
                    .speed = GPIO_SPEED_HIGH,
                    .outputType = GPIO_OUTPUT_PUSHPULL,
                    .pullType = GPIO_NOPULL,
                    .altFunc = GPIO_AF6_SPI3
                };
                ret = GPIO_Init(&spi3_ss_pin);
            }
            else{
                // Nothing -> Master mode handles SS pin via software
            }
            break;

        case SPI_4:
            // Not available on STM32F401 Discovery Board
            break;
        default:
            ret = STD_ERROR;
            break;
    }

    return ret;
}