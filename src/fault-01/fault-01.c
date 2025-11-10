#include "hal.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "simpleserial.h"

static const char flag1_password[17]; // REDACTED
static const char flag2_boot[22]; // REDACTED

static void delay() {
  for (volatile unsigned int i=0; i < 2000; i++ ){;}
}

// Flag 1 challenge
uint8_t password_check(uint8_t* input, uint8_t len) {
  uint8_t is_correct = 1;

  trigger_high();

  for (uint8_t i = 0; i < len; i++) {
    if (input[i] != flag1_password[i]) {
      is_correct = 0;
      break;
    }
    delay();
  }

  trigger_low();

  if (is_correct) {
    simpleserial_put('r', 16, (uint8_t *)"Access Granted!\0");
  } else {
    simpleserial_put('r', 16, (uint8_t *)"Access Denied!!\0");
  }

  return 0;
}

// Flag 2 challenge
void init_secure_bootloader() {
  uint32_t val_check = 0;

  trigger_high();

  for (volatile unsigned int i=0; i < 1337; i++ ){
    val_check++;
  }

  trigger_low();

  if (val_check == 1337) {
    simpleserial_put('r', 22, (uint8_t *)"Entering normal mode!");
  } else {
    // Entering secure bootloader mode
    simpleserial_put('r', 22, (uint8_t *)flag2_boot);
  }
}

int main(void) {
  platform_init();
  init_uart();
  trigger_setup();

  simpleserial_init();
  simpleserial_addcmd('p', 16, password_check); // 'p' command for password checking, takes in 16 bytes
  simpleserial_addcmd('b', 0, init_secure_bootloader); // 'b' command for bootloader check

  while (1) {
    simpleserial_get();
  }
}

