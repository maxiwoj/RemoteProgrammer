/*
 * Implements an object for targeting purpose
 *
 *                  Multiple
 * Object |  ID   | Instances | Mandatory |
 *  target  | 31025 |    Yes    |    No     |
 *
 *  Resources:
 *                         Supported    Multiple
 *  Name            | ID | Operations | Instances | Mandatory |  Type   | Range | Units |      Description      |
 *  target_type     |  1 |    R/W     |    No     |    Yes    |         |       |       |                       |
 *  firmware_url    |  2 |    R/W     |    No     |    Yes    | string  |       |       |                       |
 *  download_state  |  3 |    R       |    No     |    Yes    | integer | 0-255 |       |                       |
 *  firmware_version|  4 |    R       |    No     |    Yes    | integer |       |       |                       |
 *  flash_target    |  5 |    E       |    No     |    Yes    |         |       |       |                       |
 *  reset_target    |  6 |    E       |    No     |    Yes    |         |       |       |                       |
 *
 */

#include "liblwm2m.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "object_target.h"

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

/*
 * Multiple instance objects can use userdata to store data that will be shared between the different instances.
 * The lwm2m_object_t object structure - which represent every object of the liblwm2m as seen in the single instance
 * object - contain a chained list called instanceList with the object specific structure target_instance_t:
 */
typedef struct _target_instance_
{
    /*
     * The first two are mandatories and represent the pointer to the next instance and the ID of this one. The rest
     * is the instance scope user data (uint8_t target in this case)
     */
    struct _target_instance_ * next;   // matches lwm2m_list_t::next
    uint16_t shortID;               // matches lwm2m_list_t::id
    
    uint8_t flash_state;
    uint32_t * target_type;
    char * firmware_url;
    uint8_t download_state;
    uint32_t firmware_version;
} target_instance_t;

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
        *dataArrayP = lwm2m_data_new(4);
        if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
        *numDataP = 4;
        (*dataArrayP)[0].id = 1;
        (*dataArrayP)[1].id = 2;
        (*dataArrayP)[2].id = 3;
        (*dataArrayP)[3].id = 4;
        (*dataArrayP)[4].id = 6;
    }

    for (i = 0 ; i < *numDataP ; i++)
    {
        switch ((*dataArrayP)[i].id)
        {
        case 1:
            lwm2m_data_encode_int(targetP->target_type, *dataArrayP + i);
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
        *dataArrayP = lwm2m_data_new(6);
        if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
        *numDataP = 4;
        (*dataArrayP)[0].id = 1;
        (*dataArrayP)[1].id = 2;
        (*dataArrayP)[2].id = 3;
        (*dataArrayP)[3].id = 4;
        (*dataArrayP)[4].id = 5;
        (*dataArrayP)[5].id = 6;
        (*dataArrayP)[6].id = 7;
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
        /* we do not support writing the target_type yet
        {
            int64_t value;

            if (1 != lwm2m_data_decode_int(dataArray + i, &value))
            {
                return COAP_400_BAD_REQUEST;
            }
            targetP->target_type = (uint32_t) value;
        }
        break;*/
        case 2:
        {
            if (!dataArray[i].type == LWM2M_TYPE_STRING && !dataArray[i].type == LWM2M_TYPE_OPAQUE) {
                return COAP_400_BAD_REQUEST;  
            } 
            if (targetP->firmware_url != NULL) {
                lwm2m_free(targetP->firmware_url);
            }
            targetP->firmware_url = lwm2m_strdup((char*)dataArray[i].value.asBuffer.buffer);
            targetP->firmware_version = lwm2m_gettime();
            targetP->download_state = DOWNLOAD_IN_PROGRESS;
            // TODO: Start Downloading and add callback to set downloadCOMPLETED
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
                          lwm2m_object_t * objectP)
{
    target_instance_t * targetP;
    uint8_t result;


    targetP = (target_instance_t *)lwm2m_malloc(sizeof(target_instance_t));
    if (NULL == targetP) return COAP_500_INTERNAL_SERVER_ERROR;
    memset(targetP, 0, sizeof(target_instance_t));

    targetP->shortID = instanceId;
    objectP->instanceList = LWM2M_LIST_ADD(objectP->instanceList, targetP);

    result = target_write(instanceId, numData, dataArray, objectP);

    if (result != COAP_204_CHANGED)
    {
        (void)target_delete(instanceId, objectP);
    }
    else
    {
        result = COAP_201_CREATED;
    }

    return result;
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
        if (targetP->download_state != DOWNLOAD_COMPLETED) {
            return COAP_412_PRECONDITION_FAILED;
        }
        fprintf(stdout, "\r\n-----------------\r\n"
                        "Execute flash_target on %hu/%d/%d\r\n"
                        " Parameter (%d bytes):\r\n",
                        objectP->objID, instanceId, resourceId, length);
        prv_output_buffer((uint8_t*)buffer, length);
        fprintf(stdout, "-----------------\r\n\r\n");

// TODO: FLASH TARGET AND Update flash progress
        return COAP_204_CHANGED;
    case 6:
        return COAP_405_METHOD_NOT_ALLOWED;
    case 7:
        if (targetP->flash_state != 100 && targetP->flash_state != 0) {
            return COAP_503_SERVICE_UNAVAILABLE;
        }
        fprintf(stdout, "\r\n-----------------\r\n"
                        "Execute reset_target on %hu/%d/%d\r\n"
                        " Parameter (%d bytes):\r\n",
                        objectP->objID, instanceId, resourceId, length);
        prv_output_buffer((uint8_t*)buffer, length);
        fprintf(stdout, "-----------------\r\n\r\n");
// TODO: RESET TARGET
        return COAP_204_CHANGED;
    default:
        return COAP_404_NOT_FOUND;
    }
}

lwm2m_object_t * get_target_object(void)
{
    lwm2m_object_t * targetObj;

    targetObj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));

    if (NULL != targetObj)
    {
        int i;
        target_instance_t * targetP;

        memset(targetObj, 0, sizeof(lwm2m_object_t));

        targetObj->objID = 31025;
        
        targetP = (target_instance_t *)lwm2m_malloc(sizeof(target_instance_t));
        if (NULL == targetP) return NULL;
        memset(targetP, 0, sizeof(target_instance_t));
        targetP->shortID = 1;
        targetP->target_type = 3124;
        targetP->firmware_url = NULL;
        targetP->download_state = NO_DOWNLOAD_DATA;
        targetP->firmware_version = 0;
        targetP->flash_state = 0;

        targetObj->instanceList = LWM2M_LIST_ADD(targetObj->instanceList, targetP);
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
