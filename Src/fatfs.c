/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

#include "fatfs.h"

uint8_t retUSBH;    /* Return value for USBH */
char USBHPath[4];   /* USBH logical drive path */
FATFS USBHFatFS;    /* File system object for USBH logical drive */
FIL USBHFile;       /* File object for USBH */

/* USER CODE BEGIN Variables */

/* USER CODE END Variables */    

void MX_FATFS_Init(void) 
{
  /*## FatFS: Link the USBH driver ###########################*/
  retUSBH = FATFS_LinkDriver(&USBH_Driver, USBHPath);

  /* USER CODE BEGIN Init */
  /* additional user code for init */
  /* USER CODE END Init */
}

/**
  * @brief  Gets Time from RTC 
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
  /* USER CODE BEGIN get_fattime */
  return 0;
  /* USER CODE END get_fattime */  
}

/* USER CODE BEGIN Application */
FRESULT scan_files (
    char* path        /* Start node to be scanned (***also used as work area***) */
)
{
  printf("Scan path \"%s\"\n", path);
  FRESULT res;
  DIR dir;
  int i;
  static FILINFO fno;

  res = f_opendir(&dir, path);                       /* Open the directory */
  if (res == FR_OK) {
    for (;;) {
      res = f_readdir(&dir, &fno);                   /* Read a directory item */
      if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
      if (fno.fattrib & AM_DIR) {                    /* It is a directory */
        /*i = strlen(path);
        sprintf(&path[i], "/%s", fno.fname);
        res = scan_files(path);                    /* Enter the directory */
        /*if (res != FR_OK) break;
        path[i] = 0;*/
      } else {                                       /* It is a file. */
        printf("%s%s\n\r", path, fno.fname);
      }
    }
    f_closedir(&dir);
  }
  return res;
}

void usb_ls() {
  printf("Start ls\n");
  if (f_mount(&USBHFatFS, "", 1) == FR_OK) {
    scan_files(USBHPath);
    f_mount(NULL, "", 0);
  } else {
    f_mount(NULL, "", 0);
    printf("Error mounting USB\n");
  }
}

#define CHECK_FRESULT(result, msg, er) \
  if ((result) != FR_OK) { \
    printf("%s, error code 0x%x\n", (msg), (result)); \
    return (er); \
  }

int usb_write(const void *bytes, size_t size) {
  FRESULT result;

  
  result = f_mount(&USBHFatFS, "", 1);
  if (result == FR_OK) {
    FIL fp;
    result = f_open(&fp, "0:/temp", FA_WRITE | FA_CREATE_ALWAYS);
    CHECK_FRESULT(result, "open failed", -1);

    uint written_bytes;
    result = f_write(&fp, bytes, size, &written_bytes);
    CHECK_FRESULT(result, "write failed", -1);
    
    result = f_close(&fp);
    CHECK_FRESULT(result, "close failed", -1);

    f_mount(NULL, "", 0);
  } else {
    CHECK_FRESULT(result, "mount failed", -1);
  }

}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
