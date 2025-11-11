#ifndef STD_TYPES_H
#define STD_TYPES_H

typedef unsigned char       uint8_t;        
typedef unsigned short int  uint16_t;        
typedef unsigned long int   uint32_t;       
typedef signed char         int8_t;        
typedef signed short int    int16_t;       
typedef signed long int     int32_t;       
typedef float               float32_t;       
typedef double              float64_t;       

typedef enum {
    STD_ERROR = 0,
    STD_SUCCESS = 1,
    STD_TIMEOUT = 2
} STD_ReturnType;

#define NULL        (void *)0

#endif