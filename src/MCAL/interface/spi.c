#include "interface/MCAL/spi.h"
#include "private/MCAL/spi_priv.h"
#include "interface/MCAL/rcc.h"
#include "interface/Core/nvic.h"
#include "interface/MCAL/gpio.h"

static STD_ReturnType SPI_GPIO_Config(SPI_Number_t spiNum, SPI_Mode_t mode);

static SPI_Instance_t SPI[4] = {SPI1, SPI2, SPI3, SPI4};
static GPIO_t spi1_ss_pin;
static GPIO_t spi2_ss_pin;
static GPIO_t spi3_ss_pin;

SPI_Callback_t SPI1_Callback[4] = {NULL};

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

        // 10. Set Callback Function
        SPI1_Callback[config->spiNum] = config->trancieveCallback;

        // 11. Configure DMA if enabled
        if(config->dmaEnable == SPI_DMA_ENABLE){
            SPI[config->spiNum]->CR2 |= (1U << 1);
            SPI[config->spiNum]->CR2 |= (1U << 0);
        }
        else{
            // Nothing
        }

        // 12. Set Initial State
        spiState[config->spiNum] = SPI_STATE_READY;
        spiBuf[config->spiNum] = NULL;

        // 13. Enable SPI Peripheral
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
            switch(config->spiNum){
                case SPI_1:
                    NVIC_EnableIRQ(SPI1_IRQn);
                    break;
                case SPI_2:
                    NVIC_EnableIRQ(SPI2_IRQn);
                    break;
                case SPI_3:
                    NVIC_EnableIRQ(SPI3_IRQn);
                    break;
                case SPI_4:
                    NVIC_EnableIRQ(SPI4_IRQn);
                    break;
                default:
                    return STD_ERROR;
                    break;
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

STD_ReturnType SPI_GetTXEFlag(const SPI_Config_t* config, uint8_t* status){
    STD_ReturnType ret = STD_SUCCESS;

    if(config == NULL || status == NULL){
        ret = STD_ERROR;
    }
    else{
        *status = (SPI[config->spiNum]->SR & (1U << 1)) >> 1;
    }

    return ret;
}

STD_ReturnType SPI_GetRXNEFlag(const SPI_Config_t* config, uint8_t* status){
    STD_ReturnType ret = STD_SUCCESS;

    if(config == NULL || status == NULL){
        ret = STD_ERROR;
    }
    else{
        *status = SPI[config->spiNum]->SR & 1U;
    }

    return ret;
}

STD_ReturnType SPI_GetBusyFlag(const SPI_Config_t* config, uint8_t* status){
    STD_ReturnType ret = STD_SUCCESS;

    if(config == NULL || status == NULL){
        ret = STD_ERROR;
    }
    else{
        *status = (SPI[config->spiNum]->SR & (1U << 7)) >> 7;
    }

    return ret;
}

STD_ReturnType SPI_GetState(const SPI_Config_t* config, SPI_State_t* state){
    STD_ReturnType ret = STD_SUCCESS;

    if(config == NULL || state == NULL){
        ret = STD_ERROR;
    }
    else{
        *state = spiState[config->spiNum];
    }

    return ret;
}

STD_ReturnType SPI_SetState(const SPI_Config_t* config, SPI_State_t state){
    STD_ReturnType ret = STD_SUCCESS;

    if(config == NULL){
        ret = STD_ERROR;
    }
    else{
        spiState[config->spiNum] = state;
    }

    return ret;
}

void SPI1_IRQHandler(void){
    volatile static uint8_t whoIsNext = 0;
    if(SPI[SPI_1]->SR & (1U << 1) && whoIsNext == 0){ // TXE Interrupt
        if(spiBuf[SPI_1]->txIndex < spiBuf[SPI_1]->length){
            // Send next data
            if(((SPI[SPI_1]->CR1 >> 11) & 0x1) == SPI_DATA_FRAME_16BIT){
                *((volatile uint16_t*)&SPI[SPI_1]->DR) = *((uint16_t*)&(spiBuf[SPI_1]->txData[spiBuf[SPI_1]->txIndex]));
                spiBuf[SPI_1]->txIndex += 2;
            }
            else{
                *((volatile uint8_t*)&SPI[SPI_1]->DR) = *(uint8_t*)&(spiBuf[SPI_1]->txData[spiBuf[SPI_1]->txIndex]);
                spiBuf[SPI_1]->txIndex += 1;
            }
            whoIsNext = 1;
        }
        else{
            // All data transmitted -> wait for receiveing to complete
        }
    }
    if(SPI[SPI_1]->SR & (1U << 0) && whoIsNext == 1){ // RXNE Interrupt
        if(spiBuf[SPI_1]->rxIndex < spiBuf[SPI_1]->length){
            // Receive next data
            if(((SPI[SPI_1]->CR1 >> 11) & 0x1) == SPI_DATA_FRAME_16BIT){
                *((uint16_t*)&(spiBuf[SPI_1]->rxData[spiBuf[SPI_1]->rxIndex])) = *((volatile uint16_t*)&SPI[SPI_1]->DR);
                spiBuf[SPI_1]->rxIndex += 2;
            }
            else{
                *(uint8_t*)&(spiBuf[SPI_1]->rxData[spiBuf[SPI_1]->rxIndex]) = *((volatile uint8_t*)&SPI[SPI_1]->DR);
                spiBuf[SPI_1]->rxIndex += 1;
            }
            whoIsNext = 0;
            // if all received -> disable rxne & set state to ready & call callback
            if(spiBuf[SPI_1]->rxIndex >= spiBuf[SPI_1]->length){
                // Disable TXE & RXNE Interrupt & NVIC IRQ
                SPI[SPI_1]->CR2 &= ~((1U << 7) | (1U << 6));
                NVIC_DisableIRQ(SPI1_IRQn);

                spiState[SPI_1] = SPI_STATE_READY;
                if(NULL != SPI1_Callback[SPI_1]){
                    SPI1_Callback[SPI_1]();
                }
            }
        }
        else{

        }
    }
}

void SPI2_IRQHandler(void){
    if(NULL != SPI1_Callback[SPI_2]){
        SPI1_Callback[SPI_2]();
    }
}

void SPI3_IRQHandler(void){
    if(NULL != SPI1_Callback[SPI_3]){
        SPI1_Callback[SPI_3]();
    }
}

void SPI4_IRQHandler(void){
    if(NULL != SPI1_Callback[SPI_4]){
        SPI1_Callback[SPI_4]();
    }
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