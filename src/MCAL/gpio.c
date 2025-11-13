#include "/home/ehab/Documents/ITI_9Months/ARM/STM32F401_Drivers/include/private/MCAL/gpio_priv.h"
#include "/home/ehab/Documents/ITI_9Months/ARM/STM32F401_Drivers/include/interface/MCAL/gpio.h"

#define GPIO_PORT_MAX 5

// Array of GPIO pointers
const GPIOx_t* GPIO_PORTS[GPIO_PORT_MAX] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE};

STD_ReturnType GPIO_Init(GPIO_t* gpio){
    STD_ReturnType ret = STD_SUCCESS;
    if (gpio == NULL || gpio->port >= GPIO_PORT_MAX) {
        return STD_ERROR;
    }
    else{
        // Configure MODER
        GPIO_SetMode(gpio, gpio->mode);
        // Configure OTYPER
        GPIO_SetOutputType(gpio, gpio->outputType);
        // Configure OSPEEDR
        GPIO_SetSpeed(gpio, gpio->speed);
        // Configure PUPDR
        GPIO_SetPull(gpio, gpio->pullType);
        // Configure AFR
        GPIO_SetAltFunction(gpio, gpio->altFunc);
    }
    return ret;
}

STD_ReturnType GPIO_SetMode(GPIO_t* gpio, GPIO_Mode_t mode){
    STD_ReturnType ret = STD_SUCCESS;
    if (gpio == NULL || gpio->port >= GPIO_PORT_MAX) {
        return STD_ERROR;
    }
    else{
        GPIOx_t* GPIOx = GPIO_PORTS[gpio->port];
        gpio->mode = mode;
        GPIOx->MODER.REG &= ~((GPIO_MODER_MASK) << (gpio->pin * 2));
        GPIOx->MODER.REG |= ((mode & GPIO_MODER_MASK) << (gpio->pin * 2));
    }
    return ret;
}

STD_ReturnType GPIO_SetSpeed(GPIO_t* gpio, GPIO_Speed_t speed){
     STD_ReturnType ret = STD_SUCCESS;
    if (gpio == NULL || gpio->port >= GPIO_PORT_MAX) {
        return STD_ERROR;
    }
    else{
        GPIOx_t* GPIOx = GPIO_PORTS[gpio->port];
        gpio->speed = speed;
        GPIOx->OSPEEDR.REG &= ~((GPIO_SPEED_MASK) << (gpio->pin * 2));
        GPIOx->OSPEEDR.REG |= ((speed & GPIO_SPEED_MASK) << (gpio->pin * 2));
    }
    return ret;
}

STD_ReturnType GPIO_SetPull(GPIO_t* gpio, GPIO_Pull_t pull){
        STD_ReturnType ret = STD_SUCCESS;
    if(gpio == NULL || gpio->port >= GPIO_PORT_MAX){
        ret = STD_ERROR;
    }
    else{
        GPIOx_t* GPIOx = GPIO_PORTS[gpio->port];
        gpio->pullType = pull;
        GPIOx->PUPDR.REG &= ~((GPIO_PULL_MASK) << (gpio->pin * 2));
        GPIOx->PUPDR.REG |= ((pull & GPIO_PULL_MASK) << (gpio->pin * 2));
    }
    return ret;
}

STD_ReturnType GPIO_SetOutputType(GPIO_t* gpio, GPIO_OutputType_t outputType){
    STD_ReturnType ret = STD_SUCCESS;
    if(gpio == NULL || gpio->port >= GPIO_PORT_MAX){
        ret = STD_ERROR;
    }
    else{
        gpio->outputType = outputType;
        GPIOx_t* GPIOx = GPIO_PORTS[gpio->port];
        GPIOx->OTYPER.REG &= ~(1U << gpio->pin);
        GPIOx->OTYPER.REG |= ((outputType & 0x1U) << gpio->pin);
    }
    return ret;
}


STD_ReturnType GPIO_SetAltFunction(GPIO_t* gpio, GPIO_AltFunc_t altFunc){
    STD_ReturnType ret = STD_SUCCESS;
    if(gpio == NULL || gpio->port >= GPIO_PORT_MAX){
        ret = STD_ERROR;
    }
    else{
        GPIOx_t* GPIOx = GPIO_PORTS[gpio->port];
        gpio->altFunc = altFunc;
        if(gpio->pin < 8){
            GPIOx->AFRL.REG &= ~((0xFU) << (gpio->pin * 4));
            GPIOx->AFRL.REG |= ((altFunc & 0xFU) << (gpio->pin * 4));
        }
        else{
            GPIOx->AFRH.REG &= ~((0xFU) << ((gpio->pin - 8) * 4));
            GPIOx->AFRH.REG |= ((altFunc & 0xFU) << ((gpio->pin - 8) * 4));
        }
    }
    return ret;
}

STD_ReturnType GPIO_WritePin(GPIO_t* gpio, GPIO_PinState_t state){
    STD_ReturnType ret = STD_SUCCESS;
    if(gpio == NULL || gpio->port >= GPIO_PORT_MAX){
        ret = STD_ERROR;
    }
    else{
        GPIOx_t* GPIOx = GPIO_PORTS[gpio->port];
        if(state == GPIO_PIN_SET){
            GPIOx->BSRR.REG = (1U << gpio->pin);
        }
        else{
            GPIOx->BSRR.REG = (1U << (gpio->pin + 16));
        }
    }
    return ret;
}

STD_ReturnType GPIO_ReadPin(GPIO_t* gpio, GPIO_PinState_t* state){
    STD_ReturnType ret = STD_SUCCESS;
    if(gpio == NULL || state == NULL || gpio->port >= GPIO_PORT_MAX){
        ret = STD_ERROR;
    }
    else{
        GPIOx_t* GPIOx = GPIO_PORTS[gpio->port];
        if((GPIOx->IDR.REG & (1U << gpio->pin)) != 0){
            *state = GPIO_PIN_SET;
        }
        else{
            *state = GPIO_PIN_RESET;
        }
    }
    return ret;
}

STD_ReturnType GPIO_TogglePin(GPIO_t* gpio){
    STD_ReturnType ret = STD_SUCCESS;
    if(gpio == NULL || gpio->port >= GPIO_PORT_MAX){
        ret = STD_ERROR;
    }
    else{
        GPIOx_t* GPIOx = GPIO_PORTS[gpio->port];
        GPIOx->ODR.REG ^= (1U << gpio->pin);    
    }
    return ret;
}