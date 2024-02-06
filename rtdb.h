#ifndef __RTDB_H__
#define __RTDB_H__

#include <stdint.h>
#include "firestore.h"

firestore_err_t rtdb_init(void);
firestore_err_t rtdb_put_data(char *path, char *data);
int16_t rtdb_get_last_http_err();

#endif /* __FIRESTORE_H__ */