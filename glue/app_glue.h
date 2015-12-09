
#ifndef __APP_GLUE_H__
#define __APP_GLUE_H__

void *app_glue_create_socket(int family,int type);
int app_glue_v4_bind(void *so,unsigned int ipaddr, unsigned short port);
int app_glue_v4_connect(void *so,unsigned int ipaddr,unsigned short port);
int app_glue_v4_listen(void *so);
void app_glue_set_glueing_block(void *so,void *data);
void *app_glue_get_glueing_block(void *so);
void *app_glue_get_first_buffers_available_waiter();
void app_glue_remove_first_buffer_available_waiter(void *so);
void app_glue_init_buffers_available_waiters();
int app_glue_setsockopt(void *so, int level, int name, size_t size, void *data);

#endif /* __APP_GLUE_H__ */
