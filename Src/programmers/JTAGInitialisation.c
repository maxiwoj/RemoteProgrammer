#include "stm32f4xx_hal.h"

#define PIN_CATEGORY GPIOE
#define TCKWAIT 1

// JTAG pin functions
void tms(char value);
char tdi(char value);
int tdin(int n, int bits);
void tck(void);

void set(int pin, char value);
char get(int pin);


int TMS = GPIO_PIN_11;
int TCK = GPIO_PIN_12;
int TDI = GPIO_PIN_13;
int TDO = GPIO_PIN_14;


void JTAG_Init(){
  GPIO_InitTypeDef GPIO_InitStruct;

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(PIN_CATEGORY, TDI, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(PIN_CATEGORY, TMS, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(PIN_CATEGORY, TDO, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(PIN_CATEGORY, TCK, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = TMS | TDO | TCK; 
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(PIN_CATEGORY, &GPIO_InitStruct);


  GPIO_InitStruct.Pin = TDI;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;

  HAL_GPIO_Init(PIN_CATEGORY, &GPIO_InitStruct);

}


void enumdevs(void)
{
    int n7, n6, n5, n4, n3, n2, lsb;

    // Test-Logic-Reset
    tms(1); tms(1); tms(1); tms(1); tms(1);

    // Capture-DR
    tms(0); tms(1); tms(0);

    for(;;) {
        lsb = tdi(1);
        if(lsb == 0) {
          // NO ID CODE
            // Serial.println("0         [device has no idcode]");
        } else {
            // read manufacturer code (7+4 bits)
            lsb |= tdin(7, 0xff) << 1;
            n2 = tdin(4, 0xf);

            if(lsb == 0xff && n2 == 0xf) {
                // no such manufacturer, must be our own 1s
                // NB: actually IEEE 1149.1-2001 reserves 0000 1111111
                //     as the invalid manufacturer code, but i'm too
                //     lazy to track when to insert the zeros, so let's
                //     assume 1111 1111111 will not be used either.
                break;
            }

            // read product code
            n3 = tdin(4, 0xf);
            n4 = tdin(4, 0xf);
            n5 = tdin(4, 0xf);
            n6 = tdin(4, 0xf);
            // read product version
            n7 = tdin(4, 0xf);

            // // print in the format VPPPPMMM
            // Serial.print(out4(n7));
            // Serial.print(out4(n6));
            // Serial.print(out4(n5));
            // Serial.print(out4(n4));
            // Serial.print(out4(n3));
            // Serial.print(out4(n2));
            // Serial.print(out4(lsb >> 4));
            // Serial.print(out4(lsb & 0xf));

            // // print in the format vvvv pppppppppppppppp mmmmmmmmmmm 1
            // Serial.print("  [");
            // printbits(4, n7);
            // Serial.print(' ');
            // printbits(4, n6);
            // printbits(4, n5);
            // printbits(4, n4);
            // printbits(4, n3);
            // Serial.print(' ');
            // printbits(4, n2);
            // printbits(7, lsb >> 1);
            // Serial.print(' ');
            // printbits(1, lsb);
            // Serial.print("]");

            // Serial.println();
        }
    }

    // Run-Test/Idle
    tms(1); tms(1); tms(0);
}
// void printbits(int n, char bits)
// {
//     int b;

//     for(n--; n>=0; n--) {
//         b = (bits >> n) & 1;
//         Serial.print(b? '1' : '0');
//     }
// }


// set TMS to value, cycle TCK
void tms(char value)
{
    set(TMS, value);
    tck();
}

// cycle TCK, set TDI to value, sample TDO
char tdi(char value)
{
    tck();
    set(TDI, value);
    return get(TDO);
}

// multi-bit version of tdi()
int tdin(int n, int bits)
{
    int tmp=0, res=0;
    int i;

    // shift bits and push into tmp lifo-order
    for(i=0; i<n; i++) {
        tmp = tmp<<1 | tdi(bits & 1);
        bits >>= 1;
    }

    // reverse bit order tmp->res
    for(i=0; i<n; i++) {
        res = res<<1 | tmp&1;
        tmp >>= 1;
    }

    return res;
}

void tck(void)
{
    set(TCK, 1);
    HAL_Delay(TCKWAIT);
    set(TCK, 0);
    HAL_Delay(TCKWAIT);
}

void set(int pin, char value)
{
  HAL_GPIO_WritePin(PIN_CATEGORY, TDO, value ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

char get(int pin)
{
    return HAL_GPIO_ReadPin(PIN_CATEGORY, TDI) == GPIO_PIN_SET;
}
