#include "interface/MCAL/i2c.h"
#include "private/MCAL/i2c_priv.h"
#include "interface/MCAL/rcc.h"
#include "interface/MCAL/gpio.h"
#include "interface/Core/nvic.h"
#include "interface/Core/systick.h"

static STD_ReturnType I2C_GPIO_Init(I2C_Number_t i2cNumber);

I2C_Registers_t* I2C[3] = {I2C1, I2C2, I2C3};

STD_ReturnType I2C_Init(const I2C_Config_t* config){
    STD_ReturnType ret = STD_SUCCESS;

    if(config == NULL){
        ret = STD_ERROR;
    }
    else{
        // 1. Clock Enable for I2C Peripheral
        switch(config->i2cNumber){
            case I2C_1:
                RCC_ControlPeripheral(RCC_I2C1, RCC_PERIPHERAL_ENABLE);
                break;
            case I2C_2:
                RCC_ControlPeripheral(RCC_I2C2, RCC_PERIPHERAL_ENABLE);
                break;
            case I2C_3:
                RCC_ControlPeripheral(RCC_I2C3, RCC_PERIPHERAL_ENABLE);
                break;
            default:
                return STD_ERROR;
        }

        // 2. Set Clock Frequency
        if((config->apb1ClockFreq < I2C_MIN_CLOCK_FREQ) || (config->apb1ClockFreq > I2C_MAX_CLOCK_FREQ)){
            return STD_ERROR;
        }
        else{
            I2C[config->i2cNumber]->CR2 |= (config->apb1ClockFreq/1000000U) & 0x3F;
        }

        // 3. Set Address in Slave Mode
        if(config->mode == I2C_SLAVE_MODE){
            if(config->addrMode == I2C_10BIT_ADDR_MODE){
                I2C[config->i2cNumber]->OAR1 |= (1U << 15); // 10-bit Addressing Mode
                I2C[config->i2cNumber]->OAR1 |= (config->ownAddress & 0x3FF);
            }
            else{
                I2C[config->i2cNumber]->OAR1 &= ~(1U << 15); // 7-bit Addressing Mode
                I2C[config->i2cNumber]->OAR1 |= ((config->ownAddress & 0x7F) << 1);
            }
        }
        else{
            // Set Bus Speed
            if(config->busSpeed == I2C_BUS_SPEED_STANDARD){
                // Standard Mode (100 kHz)
                uint16_t ccrValue = (config->apb1ClockFreq) / (2 * 100000);
                if(ccrValue < 4){
                    ccrValue = 4; // Minimum CCR value
                }
                I2C[config->i2cNumber]->CCR = (ccrValue & 0xFFF);
                I2C[config->i2cNumber]->TRISE = (config->apb1ClockFreq / 1000000U) + 1;

            }
            else if(config->busSpeed == I2C_BUS_SPEED_FAST){
                // Fast Mode (400 kHz)
                uint16_t ccrValue = (config->apb1ClockFreq) / (3 * 400000);
                if(ccrValue < 1){
                    ccrValue = 1; // Minimum CCR value
                }
                I2C[config->i2cNumber]->CCR = (1U << 15) | (ccrValue & 0x0FFF);
                I2C[config->i2cNumber]->TRISE = ((config->apb1ClockFreq / 1000000U) * 300 / 1000) + 1;
            }
            else{
                return STD_ERROR;
            }
        }

        // 4. Enable I2C GPIO Pins
        ret = I2C_GPIO_Init(config->i2cNumber);

        // 5. DMA Configuration
        if(config->dmaEnable == I2C_DMA_ENABLE){
            I2C[config->i2cNumber]->CR2 |= I2C_CR2_DMAEN; // DMA Enable
        }
        else{
            // Already Disabled by Default
        }

        // 6. Enable I2C Peripheral
        I2C[config->i2cNumber]->CR1 |= I2C_CR1_PE;
    }

    return ret;
}

STD_ReturnType I2C_DeInit(const I2C_Config_t* config){
    STD_ReturnType ret = STD_SUCCESS;

    // Clear Registers
    I2C[config->i2cNumber]->CR1 = 0;
    I2C[config->i2cNumber]->CR2 = 0;
    I2C[config->i2cNumber]->OAR1 = 0;
    I2C[config->i2cNumber]->OAR2 = 0;
    I2C[config->i2cNumber]->DR = 0;
    I2C[config->i2cNumber]->SR1 = 0;
    I2C[config->i2cNumber]->SR2 = 0;
    I2C[config->i2cNumber]->CCR = 0;
    I2C[config->i2cNumber]->TRISE = 0;
    I2C[config->i2cNumber]->FLTR = 0;

    // Disable Clock for I2C Peripheral
    switch(config->i2cNumber){
        case I2C_1:
            RCC_ControlPeripheral(RCC_I2C1, RCC_PERIPHERAL_DISABLE);
            break;
        case I2C_2:
            RCC_ControlPeripheral(RCC_I2C2, RCC_PERIPHERAL_DISABLE);
            break;
        case I2C_3:
            RCC_ControlPeripheral(RCC_I2C3, RCC_PERIPHERAL_DISABLE);
            break;
        default:
            ret = STD_ERROR;
            break;
    }

    return ret;
}

STD_ReturnType I2C_Master_Transmit(const I2C_Config_t* config, uint16_t devAddress, I2C_Buffer_t* buffer, uint32_t timeoutMS){
    STD_ReturnType ret = STD_SUCCESS;

    if(config == NULL || buffer == NULL || buffer->data == NULL || buffer->length == 0){
        return STD_ERROR;
    }
    else{
        uint32_t tickStart = 0;

        // 1. Wait until BUSY flag is reset (SR2 BUSY bit)
        tickStart = 0;
        while(I2C[config->i2cNumber]->SR2 & I2C_SR2_BUSY){
            if(tickStart++ > timeoutMS){
                return STD_TIMEOUT;
            }
            SYSTICK_DelayMS(1);
        }

        // 2. Generate Start Condition (START bit)
        I2C[config->i2cNumber]->CR1 |= I2C_CR1_START;

        // 3. Wait for SB (Start Bit) flag
        tickStart = 0;
        while(!(I2C[config->i2cNumber]->SR1 & I2C_SR1_SB)){
            if(tickStart++ > timeoutMS){
                return STD_TIMEOUT;
            }
            SYSTICK_DelayMS(1);
        }

        // 4. Send Slave Address (7-bit mode)
        if(config->addrMode == I2C_10BIT_ADDR_MODE){
            // Don't Care :(
        }
        else{
            I2C[config->i2cNumber]->DR = devAddress;
        }

        // 5. Wait for ADDR flag
        tickStart = 0;
        while(!(I2C[config->i2cNumber]->SR1 & I2C_SR1_ADDR)){
            // Check for ACK Failure (AF) while waiting
            if(I2C[config->i2cNumber]->SR1 & I2C_SR1_AF){
                I2C[config->i2cNumber]->SR1 &= ~I2C_SR1_AF; // Clear AF
                I2C[config->i2cNumber]->CR1 |= I2C_CR1_STOP;   // Generate STOP
                return STD_ERROR;                           // NACK received
            }

            if(tickStart++ > timeoutMS) return STD_TIMEOUT;
            SYSTICK_DelayMS(1);
        }

        // 6. Clear ADDR flag by reading SR1 and SR2
        (void)I2C[config->i2cNumber]->SR1;
        (void)I2C[config->i2cNumber]->SR2;


        // 7. Transmit Data Bytes
        for(uint16_t i = 0; i < buffer->length; i++){
            I2C[config->i2cNumber]->DR = buffer->data[i];

            // Wait for TXE or BTF
            tickStart = 0;
            while(!(I2C[config->i2cNumber]->SR1 & I2C_SR1_TxE)){  // TXE
                if(I2C[config->i2cNumber]->SR1 & I2C_SR1_AF){   // ACK Fail
                    I2C[config->i2cNumber]->SR1 &= ~I2C_SR1_AF; // Clear AF
                    I2C[config->i2cNumber]->CR1 |= I2C_CR1_STOP;   // STOP
                    return STD_ERROR;
                }
                if(tickStart++ > timeoutMS) return STD_TIMEOUT;
                SYSTICK_DelayMS(1);
            }

            // Wait for BTF flag (Byte Transfer Finished)
            if(i + 1 < buffer->length){
                tickStart = 0;
                while(!(I2C[config->i2cNumber]->SR1 & I2C_SR1_BTF)){ // BTF
                    if(I2C[config->i2cNumber]->SR1 & I2C_SR1_AF){  // ACK Fail
                        I2C[config->i2cNumber]->SR1 &= ~I2C_SR1_AF; // Clear AF
                        I2C[config->i2cNumber]->CR1 |= I2C_CR1_STOP;   // STOP
                        return STD_ERROR;
                    }
                    if(tickStart++ > timeoutMS) return STD_TIMEOUT;
                    SYSTICK_DelayMS(1);
                }
            }
        }

        // 8. Generate Stop Condition
        I2C[config->i2cNumber]->CR1 |= I2C_CR1_STOP;
    }

    return ret;
}

STD_ReturnType I2C_Master_Receive(const I2C_Config_t* config, uint16_t devAddress, I2C_Buffer_t* buffer, uint32_t timeoutMS){
    STD_ReturnType ret = STD_SUCCESS;

    if(config == NULL || buffer == NULL || buffer->data == NULL || buffer->length == 0){
        return STD_ERROR;
    }
    else{
        uint32_t tickStart = 0;

        // 1. Wait until BUSY flag is reset (SR2 BUSY bit)
        tickStart = 0;
        while(I2C[config->i2cNumber]->SR2 & (1U << 1)){
            if(tickStart++ > timeoutMS) return STD_TIMEOUT;
            SYSTICK_DelayMS(1);
        }

        // 2. Generate Start Condition (START bit)
        I2C[config->i2cNumber]->CR1 |= (1U << 8);

        // 3. Wait for SB (Start Bit) flag
        tickStart = 0;
        while(!(I2C[config->i2cNumber]->SR1 & (1U << 0))){
            if(tickStart++ > timeoutMS) return STD_TIMEOUT;
            SYSTICK_DelayMS(1);
        }

        // 4. Send Slave Address (7-bit mode)
        if(config->addrMode == I2C_10BIT_ADDR_MODE){
            // Don't Care :(
        }
        else{
            I2C[config->i2cNumber]->DR = (devAddress << 1) | 1; // Read '1'
        }

        // 5. Wait for ADDR flag
        tickStart = 0;
        while(!(I2C[config->i2cNumber]->SR1 & (1U << 1))){
            // Check for ACK Failure (AF) while waiting
            if(I2C[config->i2cNumber]->SR1 & (1U << 10)){
                I2C[config->i2cNumber]->SR1 &= ~(1U << 10); // Clear AF
                I2C[config->i2cNumber]->CR1 |= (1U << 9);   // Generate STOP
                return STD_ERROR;                           // NACK received
            }

            if(tickStart++ > timeoutMS) return STD_TIMEOUT;
            SYSTICK_DelayMS(1);
        }

        // 6. Clear ADDR flag by reading SR1 and SR2
        volatile uint32_t temp = I2C[config->i2cNumber]->SR1;
        temp = I2C[config->i2cNumber]->SR2;

        // 7. Receive Data Bytes
        for(uint16_t i = 0; i < buffer->length; i++){
            // If this is the last byte: NACK + STOP
            if(i == (buffer->length - 1)){
                // Disable ACK (send NACK)
                I2C[config->i2cNumber]->CR1 &= ~(1U << 10);
                // Send STOP
                I2C[config->i2cNumber]->CR1 |= (1U << 9);
            }

            // Wait for RXNE
            tickStart = 0;
            while(!(I2C[config->i2cNumber]->SR1 & (1U << 6))){   // RXNE
                if(tickStart++ > timeoutMS) return STD_TIMEOUT;
                SYSTICK_DelayMS(1);
            }
            // Read the received byte
            buffer->data[i] = I2C[config->i2cNumber]->DR;
        }

        // 8. Generate Stop Condition
        I2C[config->i2cNumber]->CR1 |= (1U << 9);
    }

    return ret;
}

static STD_ReturnType I2C_GPIO_Init(I2C_Number_t i2cNumber){
    STD_ReturnType ret = STD_SUCCESS;

    GPIO_t i2c_scl;
    GPIO_t i2c_sda;

    switch(i2cNumber){
        case I2C_1:
            // Initialize GPIO Pins for I2C1 (SCL: PB6, SDA: PB7)
            i2c_scl = (GPIO_t){
                .port = GPIO_PORTB,
                .pin = GPIO_PIN_6,
                .mode = GPIO_MODE_AF,
                .speed = GPIO_SPEED_VERY_HIGH,
                .outputType = GPIO_OUTPUT_OPENDRAIN,
                .pullType = GPIO_NOPULL,
                .altFunc = GPIO_AF4_I2C1_2_3
            };
            ret = GPIO_Init(&i2c_scl);
            if(ret != STD_SUCCESS){
                return ret;
            }

            i2c_sda = (GPIO_t){
                .port = GPIO_PORTB,
                .pin = GPIO_PIN_7,
                .mode = GPIO_MODE_AF,
                .speed = GPIO_SPEED_VERY_HIGH,
                .outputType = GPIO_OUTPUT_OPENDRAIN,
                .pullType = GPIO_NOPULL,
                .altFunc = GPIO_AF4_I2C1_2_3
            };
            ret = GPIO_Init(&i2c_sda);
            if(ret != STD_SUCCESS){
                ret = STD_ERROR;
            }

            break;
        case I2C_2:
            // Initialize GPIO Pins for I2C2 (SCL: PB10, SDA: PB11)
            i2c_scl = (GPIO_t){
                .port = GPIO_PORTB,
                .pin = GPIO_PIN_10,
                .mode = GPIO_MODE_AF,
                .speed = GPIO_SPEED_VERY_HIGH,
                .outputType = GPIO_OUTPUT_OPENDRAIN,
                .pullType = GPIO_NOPULL,
                .altFunc = GPIO_AF4_I2C1_2_3
            };
            ret = GPIO_Init(&i2c_scl);
            if(ret != STD_SUCCESS){
                return ret;
            }

            i2c_sda = (GPIO_t){
                .port = GPIO_PORTB,
                .pin = GPIO_PIN_3,
                .mode = GPIO_MODE_AF,
                .speed = GPIO_SPEED_VERY_HIGH,
                .outputType = GPIO_OUTPUT_OPENDRAIN,
                .pullType = GPIO_NOPULL,
                .altFunc = GPIO_AF9_I2C2_3
            };
            ret = GPIO_Init(&i2c_sda);
            if(ret != STD_SUCCESS){
                ret = STD_ERROR;
            }

            break;
        case I2C_3:
            // Initialize GPIO Pins for I2C3 (SCL: PA8, SDA: PB4)
            i2c_scl = (GPIO_t){
                .port = GPIO_PORTA,
                .pin = GPIO_PIN_8,
                .mode = GPIO_MODE_AF,
                .speed = GPIO_SPEED_HIGH,
                .outputType = GPIO_OUTPUT_OPENDRAIN,
                .pullType = GPIO_NOPULL,
                .altFunc = GPIO_AF4_I2C1_2_3
            };
            ret = GPIO_Init(&i2c_scl);
            if(ret != STD_SUCCESS){
                return ret;
            }

            i2c_sda = (GPIO_t){
                .port = GPIO_PORTB,
                .pin = GPIO_PIN_4,
                .mode = GPIO_MODE_AF,
                .speed = GPIO_SPEED_HIGH,
                .outputType = GPIO_OUTPUT_OPENDRAIN,
                .pullType = GPIO_NOPULL,
                .altFunc = GPIO_AF9_I2C2_3
            };
            ret = GPIO_Init(&i2c_sda);
            if(ret != STD_SUCCESS){
                ret = STD_ERROR;
            }

            break;
        default:
            ret = STD_ERROR;
            break;
    }

    return ret;
}