#ifndef FRIENDS_H
#define FRIENDS_H

#include <stdint.h>

typedef uint16_t pex_uuid_t;
extern pex_uuid_t pet_pex_id;

void friends_set_pex_id(pex_uuid_t id);

#endif
