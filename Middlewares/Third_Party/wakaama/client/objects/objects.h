#ifndef __OBJECTS_H
#define __OBJECTS_H

lwm2m_object_t * get_security_object();
lwm2m_object_t * get_server_object();
lwm2m_object_t * get_object_device();
lwm2m_object_t * get_test_object();

void free_security_object();
void clean_server_object();
void free_object_device();
void free_test_object();

#define OBJ_COUNT 4

#endif