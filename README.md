# STM32F401 Drivers
### Project Structure
.
├── **include**
│   ├── **configuration**
│   │   ├── Core
│   │   │   └── nvic_conf.h
│   │   └── HAL
│   │   │   ├── led_cfg.h
│   │   │   ├── sevenseg_cfg.h
│   │   │   └── switch_cfg.h
│   ├── **interface**
│   │   ├── Core
│   │   │   └── nvic.h
│   │   ├── HAL
│   │   │   ├── led.h
│   │   │   ├── sevenseg.h
│   │   │   └── switch.h
│   │   └── MCAL
│   │   │   ├── gpio.h
│   │   │   └── rcc.h
│   └── **private**
│   │   ├── Core
│   │   │   └── nvic_priv.h
│   │   └── MCAL
│   │   │   ├── gpio_priv.h
│   │   │   └── rcc_priv.h
├── **lib**
│   ├── BIT_Math.h
│   └── STD_Types.h
└── **src**
    ├── **main.c**
    ├── **Core**
    │   ├── nvic.c
    ├── **HAL**
    │   ├── config
    │   │   ├── led_cfg.c
    │   │   ├── sevenseg_cfg.c
    │   │   └── switch_cfg.c
    │   └── interface
    │   │   ├── led.c
    │   │   │   │   ├── sevenseg.c
    │   │   └── switch.c
    └── **MCAL**
        ├── config
        └── interface
            ├── gpio.c
            └── rcc.c
