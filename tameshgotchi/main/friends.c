#include "friends.h"

pex_uuid_t pet_pex_id = 0;

void friends_set_pex_id(pex_uuid_t id) {
	pet_pex_id = id;
}
