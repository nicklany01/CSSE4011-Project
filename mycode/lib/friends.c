#include "friends.h"

pex_uuid_t pet_pex_id = 0;

static int friends_idx = 0;
static int enemies_idx = 0;

pex_uuid_t friends[FRIENDS_MAX] = {0};
pex_uuid_t enemies[FRIENDS_MAX] = {0};

void friends_set_pex_id(pex_uuid_t id) {
	if (id == PEX_ID_RESERVED_NULL) {
		return;
	}

	pet_pex_id = id;
}

static int friends_find_id(pex_uuid_t *list, pex_uuid_t id) {
	for (int i = 0; i < FRIENDS_MAX; i++) {
		if (list[i] == id) {
			return i;
		}
	}

	return -1;
}

bool friends_add_friend(pex_uuid_t id) {

	if (friends_find_id(enemies, id) != -1) {
		// cant add enemy as friend
		return false;
	}

	// find free spot
	int spot = friends_find_id(enemies, PEX_ID_RESERVED_NULL);

	if (spot == -1) {
		// no free space
		return false;
	}

	friends[spot] = id;
	return true;
}

bool friends_rem_friend(pex_uuid_t id) {
	int spot = friends_find_id(friends, id);
	if (spot == -1) {
		return false;
	}

	friends[spot] = PEX_ID_RESERVED_NULL;
	return true;
}

bool friends_add_enemy(pex_uuid_t id) {

	if (friends_find_id(friends, id) != -1) {
		// cant add friend as enemy
		return false;
	}

	// find free spot
	int spot = friends_find_id(enemies, PEX_ID_RESERVED_NULL);

	if (spot == -1) {
		// no free space
		return false;
	}

	enemies[spot] = id;
	return true;
}

bool friends_rem_enemy(pex_uuid_t id) {
	int spot = friends_find_id(enemies, id);
	if (spot == -1) {
		return false;
	}

	friends[spot] = PEX_ID_RESERVED_NULL;
	return true;
}

friendship_status_e friends_pex_id_is_what(pex_uuid_t id) {
	// table scan friends and enemies

	for (int i = 0; i < FRIENDS_MAX; i++) {
		if (friends[i] == id) {
			return FRIENDSHIP_FRIEND;
		}

		if (enemies[i] == id) {
			return FRIENDSHIP_ENEMY;
		}
	}

	return FRIENDSHIP_NONE;
}
