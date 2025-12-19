#include "interface/MCAL/uart.h"
#include "interface/MCAL/rcc.h"
#include "interface/Core/nvic.h"
#include "interface/MCAL/gpio.h"

volatile uint8_t txBuffer[3][100] = {0};
volatile uint8_t* requestedTxBuffer[3] = {NULL};
volatile uint32_t requestedTxLength[3] = {0};
volatile uint8_t  requestedTxIndex[3] = {0};
volatile uint8_t* requestedRxBuffer[3] = {NULL};
volatile uint32_t requestedRxLength[3] = {0};
volatile uint8_t  requestedRxIndex[3] = {0};

CBFunc_t txCallback[3] ={NULL};
CBFunc_t rxCallback[3] ={NULL};

static inline int8_t uart_index(const UART_Instance_t* uart)
{
    if(uart == UART1) return 0;
    if(uart == UART2) return 1;
    if(uart == UART6) return 2;
    return -1;
}

STD_ReturnType UART_Init(const UART_Config_t* uartObj, SYSTICK_ClockSource_t clockSource){
    STD_ReturnType ret = STD_SUCCESS;

    if(uartObj == NULL){
        ret = STD_ERROR;
    }
    else{
        // Get UART Instance Number
        int8_t uartNum = uart_index(uartObj->UartInstance);
        uint8_t altFunc = 0;
        if(uartNum < 0){
            ret = STD_ERROR;
            return ret;
        }
        // 0. Extract Clock Source Frequency
        uint32_t clockFreq = 0;
        switch(clockSource){
            case SYSTICK_CLOCK_SOURCE_HSI:
                clockFreq = 16000000; // 16 MHz
                break;
            case SYSTICK_CLOCK_SOURCE_HSE:
                clockFreq = 25000000;  // 25 MHz
                break;
            case SYSTICK_CLOCK_SOURCE_PLL_MAX:
                clockFreq = 84000000;  // 84 MHz
                break;
            default:
                ret = STD_ERROR;
                break;
        }
        if(uartNum == 1){
            clockFreq /= 2; // Divide Clock Frequenct by 2 for USART2 because its connected to APB1 (42MHz)
        }
        else{
            // Nothing
        }

        if(ret != STD_SUCCESS){
            return ret;
        }

        // 1. RCC Enable
        if(uartNum == 0){
            ret = RCC_ControlPeripheral(RCC_USART1, RCC_PERIPHERAL_ENABLE);
            altFunc = GPIO_AF7_USART1_2;
        }
        else if(uartNum == 1){
            ret = RCC_ControlPeripheral(RCC_USART2, RCC_PERIPHERAL_ENABLE);
            altFunc = GPIO_AF7_USART1_2;
        }
        else if(uartNum == 2){
            ret = RCC_ControlPeripheral(RCC_USART6, RCC_PERIPHERAL_ENABLE);
            altFunc = GPIO_AF8_USART6;
        }
        else{
            ret = STD_ERROR;
        }

        // 2. Baud Rate Selection
        uint32_t baudRate = uartObj->BaudRate;
        uint16_t usartdiv = 0;
        uint8_t fraction = 0;

        usartdiv =(clockFreq) /(16 * baudRate);   // oversampling by 16(OVER8 = 0)
        fraction =((clockFreq %(16 * baudRate)) * 16 +(16 * baudRate)/2) /(16 * baudRate);  // get rem*16 "to be from 0:15" then rounding Rounding
        usartdiv =(usartdiv << 4) | fraction;
        uartObj->UartInstance->BRR = usartdiv;

        // 3. Configure Data Bits
        if(uartObj->DataBits == UART_DATABITS_8){
            uartObj->UartInstance->CR1 &= ~(1 << 12);
        }
        else{
            uartObj->UartInstance->CR1 |=(1 << 12);
        }

        // 4. Configure Parity
        if(uartObj->Parity == UART_PARITY_NONE){
            uartObj->UartInstance->CR1 &= ~(1 << 10);
        }
        else if(uartObj->Parity == UART_PARITY_EVEN){
            uartObj->UartInstance->CR1 |=(1 << 10);
            uartObj->UartInstance->CR1 &= ~(1 << 9);
        }
        else{
            uartObj->UartInstance->CR1 |=(1 << 10);
            uartObj->UartInstance->CR1 |=(1 << 9);
        }

        // 5. Interrupt Configuration
        txCallback[uartNum] = uartObj->txCallback;
        rxCallback[uartNum] = uartObj->rxCallback;

        // 6. Configure Stop Bits
        uartObj->UartInstance->CR2 &= ~(0b11 << 12); // 1 Stop Bit

        // 7. Enable Transmitter and Receiver
        uartObj->UartInstance->CR1 |=(1 << 3);  // Transmitter Enable
        uartObj->UartInstance->CR1 |=(1 << 2);  // Receiver Enable

        // 8. Enable UART
        uartObj->UartInstance->CR1 |=(1 << 13); // USART Enable

        // 9. Configure UART Pins
        GPIO_t uart_tx_pin = {
            .port       = uartObj->port,
            .pin        = uartObj->txPin,
            .mode       = GPIO_MODE_AF,
            .outputType = GPIO_OUTPUT_PUSHPULL,
            .speed      = GPIO_SPEED_HIGH,
            .pullType   = GPIO_NOPULL,
            .altFunc    = altFunc
        };
        ret = GPIO_Init(&uart_tx_pin);
        if(ret == STD_SUCCESS){
            GPIO_t uart_rx_pin = {
                .port       = uartObj->port,
                .pin        = uartObj->txPin + 1,
                .mode       = GPIO_MODE_AF,
                .outputType = GPIO_OUTPUT_PUSHPULL,
                .speed      = GPIO_SPEED_HIGH,
                .pullType   = GPIO_NOPULL,
                .altFunc    = altFunc
            };
            ret = GPIO_Init(&uart_rx_pin);
        }
        else{
            ret = STD_ERROR;
        }

        // 11. Enable NVIC
        if(uartNum == 0){
            ret = NVIC_EnableIRQ(USART1_IRQn);
        }
        else if(uartNum == 1){
            ret = NVIC_EnableIRQ(USART2_IRQn);
        }
        else if(uartNum == 2){
            ret = NVIC_EnableIRQ(USART6_IRQn);
        }
        else{
            ret = STD_ERROR;
        }
    }
    return ret;
}

STD_ReturnType UART_DeInit(const UART_Config_t* uartObj){
    STD_ReturnType ret = STD_SUCCESS;
    
    if(uartObj == NULL){
        ret = STD_ERROR;
    }
    else{
        uartObj->UartInstance->CR1 = 0U;  // Reset Control Registers 1
        uartObj->UartInstance->CR2 = 0U;  // Reset Control Registers 2
        uartObj->UartInstance->CR3 = 0U;  // Reset Control Registers 3
        uartObj->UartInstance->BRR = 0U;  // Reset Baud Rate Register

        // RCC Disable
        int8_t uartNum = uart_index(uartObj->UartInstance);
        if(uartNum == 0){
            ret = RCC_ControlPeripheral(RCC_USART1, RCC_PERIPHERAL_DISABLE);
        }
        else if(uartNum == 1){
            ret = RCC_ControlPeripheral(RCC_USART2, RCC_PERIPHERAL_DISABLE);
        }
        else if(uartNum == 2){
            ret = RCC_ControlPeripheral(RCC_USART6, RCC_PERIPHERAL_DISABLE);
        }
        else{
            ret = STD_ERROR;
        }

        // NVIC Disable
        if(uartNum == 0){
            ret = NVIC_DisableIRQ(USART1_IRQn);
        }
        else if(uartNum == 1){
            ret = NVIC_DisableIRQ(USART2_IRQn);
        }
        else if(uartNum == 2){
            ret = NVIC_DisableIRQ(USART6_IRQn);
        }
        else{
            ret = STD_ERROR;
        }
    }
    return ret;
}

STD_ReturnType UART_SendChar(const UART_Config_t* uartObj, uint8_t data, uint32_t timeoutMS){
    STD_ReturnType ret = STD_SUCCESS;
    uint32_t currentTime = 0;

    if(uartObj == NULL){
        ret = STD_ERROR;
    }
    else{
        // Wait until Transmit Data Register Empty
        while((!(uartObj->UartInstance->SR &(1 << 7))) && currentTime < timeoutMS){
            currentTime++;
            SYSTICK_DelayMS(1);
        }
        if(currentTime >= timeoutMS){
            ret = STD_TIMEOUT;
            return ret;
        }
        // Send Data
        uartObj->UartInstance->DR = data;
        // Wait until Transmission Complete
        currentTime = 0;
        while(!(uartObj->UartInstance->SR &(1 << 6)) && currentTime < timeoutMS){
            currentTime++;
            SYSTICK_DelayMS(1);
        }
        if(currentTime >= timeoutMS){
            return STD_TIMEOUT;
        }

    }
    return ret;
}

STD_ReturnType UART_SendBuffer(const UART_Config_t* uartObj, Buffer_t* buffer, uint32_t timeoutMS){
    STD_ReturnType ret = STD_SUCCESS;
    
    if(uartObj == NULL || buffer == NULL){
        ret = STD_ERROR;
    }
    else{
        while(buffer->length--){
            ret = UART_SendChar(uartObj, *buffer->data++, timeoutMS);
            if(ret != STD_SUCCESS){
                break;
            }
        }
    }
    return ret;
}

STD_ReturnType UART_ReceiveChar(const UART_Config_t* uartObj, uint8_t* data, uint32_t timeoutMS){
    STD_ReturnType ret = STD_SUCCESS;
    uint32_t currentTime = 0;
    
    if(uartObj == NULL || data == NULL){
        ret = STD_ERROR;
    }
    else{
        // Wait until Read Data Register Not Empty
        while((!(uartObj->UartInstance->SR &(1 << 5))) && currentTime < timeoutMS){
            currentTime++;
            SYSTICK_DelayMS(1);
        }
        if(currentTime >= timeoutMS){
            ret = STD_TIMEOUT;
            return ret;
        }
        // Read Data
        *data =(uint8_t)(uartObj->UartInstance->DR & 0xFF);
    }
    return ret;
}

STD_ReturnType UART_ReceiveBuffer(const UART_Config_t* uartObj, Buffer_t* buffer, uint32_t timeoutMS){
    STD_ReturnType ret = STD_SUCCESS;
    
    if(uartObj == NULL || buffer == NULL){
        ret = STD_ERROR;
    }
    else{
        while(buffer->length--){
            ret = UART_ReceiveChar(uartObj, buffer->data++, timeoutMS);
            if(ret != STD_SUCCESS){
                break;
            }
        }
    }
    return ret;
}


STD_ReturnType UART_SendCharIT(const UART_Config_t* uartObj, uint8_t data){
    STD_ReturnType ret = STD_SUCCESS;
    if(uartObj == NULL){    
        return STD_ERROR;
    }

    int8_t uartNum = uart_index(uartObj->UartInstance);
    if(uartNum < 0){
        return STD_ERROR;
    }
    if(requestedTxBuffer[uartNum] != NULL){
        return STD_BUSY;
    }

    txBuffer[uartNum][0] = data;
    requestedTxBuffer[uartNum] = &txBuffer[uartNum][0];
    requestedTxLength[uartNum] = 1;

    uartObj->UartInstance->CR1 |=(1 << 7);

    return ret;
}

STD_ReturnType UART_SendBufferIT(const UART_Config_t* uartObj, Buffer_t* buffer){
    if(uartObj == NULL || buffer == NULL || buffer->length == 0){
        return STD_ERROR;
    }
    int8_t uartNum = uart_index(uartObj->UartInstance);
    if(uartNum < 0){
        return STD_ERROR;
    }

    if(requestedTxBuffer[uartNum] != NULL){
        return STD_BUSY;
    }

    if(buffer->length > sizeof(txBuffer[uartNum])){
        return STD_ERROR;
    }

    for(uint8_t i = 0; i < buffer->length; i++){
        txBuffer[uartNum][i] = buffer->data[i];
    }
    requestedTxBuffer[uartNum] = &txBuffer[uartNum][0];
    requestedTxLength[uartNum] = buffer->length;
    buffer->index = requestedTxIndex[uartNum] = 0;

    // enable TXE
    uartObj->UartInstance->CR1 |=(1 << 7);

    return STD_SUCCESS;
}

STD_ReturnType UART_ReceiveCharIT(const UART_Config_t* uartObj, uint8_t* data){
    STD_ReturnType ret = STD_SUCCESS;

    if(uartObj == NULL || data == NULL){
        ret = STD_ERROR;
    }
    else{
        int8_t uartNum = uart_index(uartObj->UartInstance);
        if(uartNum < 0){ 
            ret = STD_ERROR;
        }
        else{
            requestedRxBuffer[uartNum] = data;
            requestedRxLength[uartNum] = 1;
            // Enable RXNE interrupt
            uartObj->UartInstance->CR1 |=(1 << 5);
        }
    }

    return ret;
}

STD_ReturnType UART_ReceiveBufferIT(const UART_Config_t* uartObj, Buffer_t* buffer){
    STD_ReturnType ret = STD_SUCCESS;

    if(uartObj == NULL || buffer == NULL || buffer->length == 0){
        ret = STD_ERROR;
    }
    else{
        int8_t uartNum = uart_index(uartObj->UartInstance);
        if(uartNum < 0){
            ret = STD_ERROR;
        }
        else{
            if(requestedRxBuffer[uartNum] != NULL){
                return STD_BUSY;
            }
            else{
                requestedRxBuffer[uartNum] = buffer->data;
                requestedRxLength[uartNum] = buffer->length;
                buffer->index = requestedRxIndex[uartNum] = 0;
                
                uartObj->UartInstance->CR1 |=(1 << 5); // enable RXNEIE
            }
        }
    }
    
    return ret;
}

void USART1_IRQHandler(void){
    // TXE handling
    if(UART1->SR &(1 << 7) && (UART1->CR1 &(1 << 7))){
        int8_t uartNum = 0;
        if(requestedTxLength[uartNum] > 0 && requestedTxBuffer[uartNum] != NULL){
            UART1->DR = *(requestedTxBuffer[uartNum]++);
            requestedTxLength[uartNum]--;
            requestedTxIndex[uartNum]++;
        }
        if(requestedTxLength[uartNum] == 0){
            UART1->CR1 &= ~(1 << 7);  // disable TXE
            requestedTxBuffer[uartNum] = NULL;
            if(txCallback[uartNum] != NULL){
                txCallback[uartNum]();
            }
        }
    }
    // RXNE handling
    if(UART1->SR &(1 << 5) && (UART1->CR1 &(1 << 5))){
        int8_t uartNum = 0;
        static uint8_t counter = 0;
        uint8_t data =(uint8_t)(UART1->DR & 0xFF);
        requestedRxBuffer[uartNum][counter++] = data;
        if(requestedRxLength[uartNum] > 0){
            *(requestedRxBuffer[uartNum]++) = data;      //////////
            requestedRxLength[uartNum]--;
            requestedRxIndex[uartNum]++;
        }
        if(requestedRxLength[uartNum] == 0){
            counter = 0;
            UART1->CR1 &= ~(1 << 5);
            requestedRxBuffer[uartNum] = NULL;
            if(rxCallback[uartNum] != NULL){
                rxCallback[uartNum]();
            }
        }
    }
}

void USART2_IRQHandler(void){
    // TXE handling
    if(UART2->SR &(1 << 7)){
        int8_t uartNum = 1;
        if(requestedTxLength[uartNum] > 0 && requestedTxBuffer[uartNum] != NULL){
            UART2->DR = *(requestedTxBuffer[uartNum]++);
            requestedTxLength[uartNum]--;
            requestedTxIndex[uartNum]++;
        }
        if(requestedTxLength[uartNum] == 0){
            UART2->CR1 &= ~(1 << 7);  // disable TXE
            requestedTxBuffer[uartNum] = NULL;
            if(txCallback[uartNum] != NULL){
                txCallback[uartNum]();
            }
        }
    }
    // RXNE handling
    if(UART2->SR &(1 << 5)){
        int8_t uartNum = 1;
        uint8_t data =(uint8_t)(UART2->DR & 0xFF);
        if(requestedRxLength[uartNum] > 0){
            *(requestedRxBuffer[uartNum]++) = data;
            requestedRxLength[uartNum]--;
            requestedRxIndex[uartNum]++;
        }
        if(requestedRxLength[uartNum] == 0){
            UART2->CR1 &= ~(1 << 5);
            requestedRxBuffer[uartNum] = NULL;
            if(rxCallback[uartNum] != NULL){
                rxCallback[uartNum]();
            }
        }
    }
}

void USART6_IRQHandler(void){
    // TXE handling
    if(UART6->SR &(1 << 7)){
        int8_t uartNum = 2;
        if(requestedTxLength[uartNum] > 0 && requestedTxBuffer[uartNum] != NULL){
            UART6->DR = *(requestedTxBuffer[uartNum]++);
            requestedTxLength[uartNum]--;
            requestedTxIndex[uartNum]++;
        }
        if(requestedTxLength[uartNum] == 0){
            UART6->CR1 &= ~(1 << 7);  // disable TXE
            requestedTxBuffer[uartNum] = NULL;
            if(txCallback[uartNum] != NULL){
                txCallback[uartNum]();
            }
        }
    }
    // RXNE handling
    if(UART6->SR &(1 << 5)){
        int8_t uartNum = 2;
        uint8_t data =(uint8_t)(UART6->DR & 0xFF);
        if(requestedRxLength[uartNum] > 0){
            *(requestedRxBuffer[uartNum]++) = data;
            requestedRxLength[uartNum]--;
        }
        if(requestedRxLength[uartNum] == 0){
            UART6->CR1 &= ~(1 << 5);
            requestedRxBuffer[uartNum] = NULL;
            if(rxCallback[uartNum] != NULL){
                rxCallback[uartNum]();
            }
        }
    }
}
