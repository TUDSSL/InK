
#ifndef GLOBAL_H_
#define GLOBAL_H_

#define DEBUG       0
#define SPI         0

#define RAMP        0

#define ROBOT       1
#define PERSISTENT  1

typedef struct nv_var {
    uint8_t cp;
    uint16_t arg;
    int16_t cnt;
    float ang;
    uint8_t stop;
}nv_var;

void swap(nv_var *a, nv_var *b);

#endif /* GLOBAL_H_ */
