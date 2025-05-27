#ifndef FRIENDS_H
#define FRIENDS_H

#include <stdint.h>
#include <stdbool.h>

#define FRIENDS_MAX 16
#define ENEMIES_MAX 16

#define PEX_ID_RESERVED_NULL 0x0000

typedef uint16_t pex_uuid_t;
extern pex_uuid_t pet_pex_id;

extern pex_uuid_t friends[FRIENDS_MAX];
extern pex_uuid_t enemies[FRIENDS_MAX];

typedef enum {

	FRIENDSHIP_NONE,
	FRIENDSHIP_FRIEND,
	FRIENDSHIP_ENEMY
} friendship_status_e;

void friends_set_pex_id(pex_uuid_t id);
friendship_status_e friends_pex_id_is_what(pex_uuid_t id);

bool friends_add_friend(pex_uuid_t id);
bool friends_add_enemy(pex_uuid_t id);
bool friends_rem_friend(pex_uuid_t id);
bool friends_rem_enemy(pex_uuid_t id);

#endif
