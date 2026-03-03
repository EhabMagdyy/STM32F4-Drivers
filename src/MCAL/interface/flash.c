#include "interface/MCAL/flash.h"
#include "private/MCAL/flash_priv.h"

STD_ReturnType FLASH_Read(uint32_t address, uint32_t* data){
    if(data == NULL){
        return STD_ERROR;
    }

    // Check if address is valid
    if(address < FLASH_SECTOR_0 || address > FLASH_SECTOR_7_END_ADDRESS){
        return STD_ERROR;
    }

    // Read data from flash memory
    *data = *((volatile uint32_t*)address);

    return STD_SUCCESS;
}

STD_ReturnType FLASH_Write(uint32_t address, uint32_t* data, uint32_t length){
    if(data == NULL || length == 0){
        return STD_ERROR;
    }

    // Check if address is valid
    if(address < FLASH_SECTOR_0 || address > FLASH_SECTOR_7_END_ADDRESS){
        return STD_ERROR;
    }

    return STD_SUCCESS;
}

STD_ReturnType FLASH_Erase(FLASH_Sector_t sectorAddress){
    // Check if sector address is valid
    if(sectorAddress < FLASH_SECTOR_0 || sectorAddress > FLASH_SECTOR_7){
        return STD_ERROR;
    }

    return STD_SUCCESS;
}

STD_ReturnType FLASH_SetLatency(FLASH_Latency_t latency){
    if(latency > 5){
        return STD_ERROR;
    }

    FLASH->ACR &= ~(0x7);  // Clear latency bits
    FLASH->ACR |= latency; // Set new latency

    return STD_SUCCESS;
}