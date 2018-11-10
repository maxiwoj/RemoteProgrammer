#ifndef __OBJECT_TARGET_H
#define __OBJECT_TARGET_H


#include "target.h"

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
    char * target_type;
    char * firmware_url;
    uint8_t download_state;
    int16_t download_error;
    uint32_t firmware_version;
    char * binary_filename;
    uint8_t download_progress;
    uint8_t flash_error;
    TARGET_t *target;
} target_instance_t;


#endif