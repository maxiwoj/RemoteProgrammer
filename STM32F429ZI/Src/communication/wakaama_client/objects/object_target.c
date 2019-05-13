/* Copyright 2018 Maksymilian Wojczuk and Tomasz Michalec
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
 * Implements an object for targeting purpose
 *
 *                  Multiple
 *   Object |  ID   | Instances | Mandatory |
 *  target  | 31025 |    Yes    |    No     |
 *
 *  Resources:
 *                         Supported    Multiple
 *  Name            | ID | Operations | Instances | Mandatory |  Type   | Range | Units |           Description         |
 * -----------------|----|------------|-----------|-----------|---------|-------|-------|-------------------------------|
 * target_type      |  1 |    R/W     |    No     |    Yes    | string  |       |       | type of the programmable board|
 * firmware_url     |  2 |    R/W     |    No     |    Yes    | string  |       |       | url to the binary             |
 * download_state   |  3 |    R       |    No     |    Yes    | integer | 0-255 |       | state of the download         |
 * firmware_version |  4 |    R       |    No     |    Yes    | integer |       |       | timestamp of the latest binary|
 * flash_target     |  5 |    E       |    No     |    Yes    |    -    |       |       | programms the target          |
 * flash_state      |  6 |    R       |    No     |    Yes    | integer |       |       | state of the last flashing    |
 * reset_target     |  7 |    E       |    No     |    Yes    |    -    |       |       | resets the target             |
 * download_error   |  8 |    R       |    No     |    Yes    | integer |       |       | download error                |
 * download_progress|  9 |    R       |    No     |    Yes    | integer | 0-100 |   %   | progress of binary download   |
 * flash_error      |  10|    R       |    No     |    Yes    | integer |       |       | flashing error                |
 * flash_progress   |  11|    R       |    No     |    Yes    | integer | 0-100 |   %   | progress of flashing the board|
 *
 */

#include "liblwm2m.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "object_target.h"
#include "binary_download.h"

int USB_BUSY = 0;
int JTAG_BUSY = 0;

static void prv_output_buffer(uint8_t * buffer,
                              int length)
{
    int i;
    uint8_t array[16];

    i = 0;
    while (i < length)
    {
        int j;
        fprintf(stderr, "  ");

        memcpy(array, buffer+i, 16);

        for (j = 0 ; j < 16 && i+j < length; j++)
        {
            fprintf(stderr, "%02X ", array[j]);
        }
        while (j < 16)
        {
            fprintf(stderr, "   ");
            j++;
        }
        fprintf(stderr, "  ");
        for (j = 0 ; j < 16 && i+j < length; j++)
        {
            if (isprint(array[j]))
                fprintf(stderr, "%c ", array[j]);
            else
                fprintf(stderr, ". ");
        }
        fprintf(stderr, "\n");

        i += 16;
    }
}

static uint8_t target_read(uint16_t instanceId,
                        int * numDataP,
                        lwm2m_data_t ** dataArrayP,
                        lwm2m_object_t * objectP)
{
    target_instance_t * targetP;
    int i;

    targetP = (target_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
    if (NULL == targetP) return COAP_404_NOT_FOUND;

    if (*numDataP == 0)
    {
        *dataArrayP = lwm2m_data_new(9);
        if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
        *numDataP = 4;
        (*dataArrayP)[0].id = 1;
        (*dataArrayP)[1].id = 2;
        (*dataArrayP)[2].id = 3;
        (*dataArrayP)[3].id = 4;
        (*dataArrayP)[4].id = 6;
        (*dataArrayP)[5].id = 8;
        (*dataArrayP)[6].id = 9;
        (*dataArrayP)[7].id = 10;
        (*dataArrayP)[8].id = 11;
    }

    for (i = 0 ; i < *numDataP ; i++)
    {
        switch ((*dataArrayP)[i].id)
        {
        case 1:
            lwm2m_data_encode_string(targetP->target_type, *dataArrayP + i);
            break;
        case 2:
            lwm2m_data_encode_string(targetP->firmware_url, *dataArrayP + i);
            break;
        case 3:
            lwm2m_data_encode_int(targetP->download_state, *dataArrayP + i);
            break;
        case 4:
            lwm2m_data_encode_int(targetP->firmware_version, *dataArrayP + i);
            break;
        case 5:
            return COAP_405_METHOD_NOT_ALLOWED;
        case 6:
            lwm2m_data_encode_int(targetP->flash_state, *dataArrayP + i);
            break;
        case 7: 
            return COAP_405_METHOD_NOT_ALLOWED;
        case 8:
            lwm2m_data_encode_int(targetP->download_error, *dataArrayP + i);
            break;
        case 9:
            lwm2m_data_encode_int(targetP->download_progress, *dataArrayP + i);
            break;
        case 10:
            lwm2m_data_encode_int(targetP->flash_error, *dataArrayP + i);
            break;
        case 11:
            lwm2m_data_encode_int(targetP->flash_progress, *dataArrayP + i);
            break;
        default:
            return COAP_404_NOT_FOUND;
        }
    }

    return COAP_205_CONTENT;
}

static uint8_t target_discover(uint16_t instanceId,
                            int * numDataP,
                            lwm2m_data_t ** dataArrayP,
                            lwm2m_object_t * objectP)
{
    int i;

    // is the server asking for the full object ?
    if (*numDataP == 0)
    {
        *dataArrayP = lwm2m_data_new(11);
        if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
        *numDataP = 8;
        (*dataArrayP)[0].id = 1;
        (*dataArrayP)[1].id = 2;
        (*dataArrayP)[2].id = 3;
        (*dataArrayP)[3].id = 4;
        (*dataArrayP)[4].id = 5;
        (*dataArrayP)[5].id = 6;
        (*dataArrayP)[6].id = 7;
        (*dataArrayP)[7].id = 8;
        (*dataArrayP)[8].id = 9;
        (*dataArrayP)[9].id = 10;
        (*dataArrayP)[10].id = 11;
    }
    else
    {
        for (i = 0; i < *numDataP; i++)
        {
            switch ((*dataArrayP)[i].id)
            {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
                break;
            default:
                return COAP_404_NOT_FOUND;
            }
        }
    }
    return COAP_205_CONTENT;
}

static uint8_t target_write(uint16_t instanceId,
                         int numData,
                         lwm2m_data_t * dataArray,
                         lwm2m_object_t * objectP)
{
    target_instance_t * targetP;
    int i;

    targetP = (target_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
    if (NULL == targetP) return COAP_404_NOT_FOUND;

    for (i = 0 ; i < numData ; i++)
    {
        switch (dataArray[i].id)
        {
        case 1:
            return COAP_405_METHOD_NOT_ALLOWED;
        case 2:
        {
            if (targetP->download_state == DOWNLOAD_IN_PROGRESS || targetP->flash_state == FLASH_IN_PROGRESS || USB_BUSY) {
                return COAP_412_PRECONDITION_FAILED;
            }
            if (!(dataArray[i].type == LWM2M_TYPE_STRING) && !(dataArray[i].type == LWM2M_TYPE_OPAQUE)) {
                return COAP_400_BAD_REQUEST;  
            } 
            if (targetP->firmware_url != NULL) {
                lwm2m_free(targetP->firmware_url);
            }
            dataArray[i].value.asBuffer.buffer[dataArray[i].value.asBuffer.length] = '\0';
            targetP->firmware_url = lwm2m_strdup((char*)dataArray[i].value.asBuffer.buffer);
            targetP->firmware_version = lwm2m_gettime();
            targetP->download_state = DOWNLOAD_IN_PROGRESS;
            sprintf(targetP->binary_filename, "%x.%d", targetP->firmware_version, targetP->shortID);
            targetP->download_progress = 0;

            USB_BUSY = 1;
            xTaskCreate(startDownload, NULL, 2000, (void*) targetP, 2, NULL);
        }
        break;
        case 3:
            return COAP_405_METHOD_NOT_ALLOWED;
        case 4:
            return COAP_405_METHOD_NOT_ALLOWED;
        case 5:
            return COAP_405_METHOD_NOT_ALLOWED;
        case 6:
            return COAP_405_METHOD_NOT_ALLOWED;
        case 7: 
            return COAP_405_METHOD_NOT_ALLOWED;
        case 8:
            return COAP_405_METHOD_NOT_ALLOWED;
        case 9:
            return COAP_405_METHOD_NOT_ALLOWED;
        case 10:
            return COAP_405_METHOD_NOT_ALLOWED;
        case 11:
            return COAP_405_METHOD_NOT_ALLOWED;
        default:
            return COAP_404_NOT_FOUND;
        }
    }

    return COAP_204_CHANGED;
}

static uint8_t target_delete(uint16_t id,
                          lwm2m_object_t * objectP)
{
    target_instance_t * targetP;

    objectP->instanceList = lwm2m_list_remove(objectP->instanceList, id, (lwm2m_list_t **)&targetP);
    if (NULL == targetP) return COAP_404_NOT_FOUND;

    lwm2m_free(targetP);

    return COAP_202_DELETED;
}

static uint8_t target_create(uint16_t instanceId,
                          int numData,
                          lwm2m_data_t * dataArray,
                          lwm2m_object_t * objectP) {
    return COAP_405_METHOD_NOT_ALLOWED;
}

static uint8_t target_exec(uint16_t instanceId,
                        uint16_t resourceId,
                        uint8_t * buffer,
                        int length,
                        lwm2m_object_t * objectP)
{
    target_instance_t * targetP;

    targetP = (target_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
    if (NULL == targetP) return COAP_404_NOT_FOUND;


    switch (resourceId)
    {
    case 1:
        return COAP_405_METHOD_NOT_ALLOWED;
    case 2:
        return COAP_405_METHOD_NOT_ALLOWED;
    case 3:
        return COAP_405_METHOD_NOT_ALLOWED;
    case 4:
        return COAP_405_METHOD_NOT_ALLOWED;
    case 5:
        if (targetP->download_state != DOWNLOAD_COMPLETED || targetP->flash_state == FLASH_IN_PROGRESS || JTAG_BUSY || USB_BUSY) {
            return COAP_412_PRECONDITION_FAILED;
        }
        fprintf(stdout, "\r\n-----------------\r\n"
                        "Executing flash_target on %hu/%d/%d\r\n"
                        "Target Name: %s\n"
                        " Parameter (%d bytes):\r\n",
                        objectP->objID, instanceId, resourceId, targetP->target_type, length);
        prv_output_buffer((uint8_t*)buffer, length);
        fprintf(stdout, "-----------------\r\n\r\n");
        targetP->flash_state=FLASH_IN_PROGRESS;
        targetP->flash_progress=0;
        JTAG_BUSY = 1;
        USB_BUSY = 1;
        xTaskCreate(flash_target_task, "Flash_Target", 2000, targetP, 1, NULL);
        return COAP_204_CHANGED;
    case 6:
        return COAP_405_METHOD_NOT_ALLOWED;
    case 7:
        if (targetP->flash_state == FLASH_IN_PROGRESS || JTAG_BUSY) {
            return COAP_412_PRECONDITION_FAILED;
        }
        fprintf(stdout, "\r\n-----------------\r\n"
                        "Execute reset_target on %hu/%d/%d\r\n"
                        "Target Name: %s\n"
                        " Parameter (%d bytes):\r\n",
                        objectP->objID, instanceId, resourceId, targetP->target_type, length);
        prv_output_buffer((uint8_t*)buffer, length);
        fprintf(stdout, "-----------------\r\n\r\n");
        JTAG_BUSY = 1;
        xTaskCreate(reset_target_task, "ResetTarget", 300, targetP, 1, NULL);
        return COAP_204_CHANGED;
    case 8:
        return COAP_405_METHOD_NOT_ALLOWED;
    case 9:
        return COAP_405_METHOD_NOT_ALLOWED;
    case 10:
        return COAP_405_METHOD_NOT_ALLOWED;
    case 11:
        return COAP_405_METHOD_NOT_ALLOWED;
    default:
        return COAP_404_NOT_FOUND;
    }
}

lwm2m_object_t * get_target_object(void)
{
    lwm2m_object_t * targetObj;
    targetObj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t)); 

        if (NULL != targetObj) {
            target_instance_t * targetP;
            int i = 1;
            memset(targetObj, 0, sizeof(lwm2m_object_t));
            targetObj->objID = 31025;

            TARGET_t *target = target_list.next;
            while(target != NULL) {
                targetP = (target_instance_t *)lwm2m_malloc(sizeof(target_instance_t));
                if (NULL == targetP) return NULL;
                memset(targetP, 0, sizeof(target_instance_t));
                targetP->shortID = (uint16_t) i;
                targetP->target_type = target->name;
                targetP->firmware_url = NULL;
                targetP->download_state = NO_DOWNLOAD_DATA;
                targetP->firmware_version = 0;
                targetP->flash_state = NO_FLASH_DATA;
                targetP->flash_progress = 0;
                targetP->download_progress = 0;
                targetP->download_error = NO_ERROR;
                targetP->binary_filename = lwm2m_malloc(25);
                targetP->target = target;

                targetObj->instanceList = LWM2M_LIST_ADD(targetObj->instanceList, targetP);

                printf("Add targetObject %d: %s\n", i, targetP->target_type);
                target = target->next;
                i++;
            }


            /*
             * From a single instance object, two more functions are available.
             * - The first one (createFunc) create a new instance and filled it with the provided informations. If an ID is
             *   provided a check is done for verifying his disponibility, or a new one is generated.
             * - The other one (deleteFunc) delete an instance by removing it from the instance list (and freeing the memory
             *   allocated to it)
             */
            targetObj->readFunc = target_read;
            targetObj->writeFunc = target_write;
            targetObj->executeFunc = target_exec;
            targetObj->createFunc = target_create;
            targetObj->deleteFunc = target_delete;
            targetObj->discoverFunc = target_discover;        
        }
    return targetObj;
}

void free_target_object(lwm2m_object_t * object)
{
    LWM2M_LIST_FREE(object->instanceList);
    if (object->userData != NULL)
    {
        lwm2m_free(object->userData);
        object->userData = NULL;
    }
    lwm2m_free(object);
}
