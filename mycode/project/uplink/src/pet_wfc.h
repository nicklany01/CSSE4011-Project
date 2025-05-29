#ifndef PET_WFC_H
#define PET_WFC_H

typedef enum {

	PET_WFC_NULL,
	PET_WFC_SEND_HELLO,
	PET_WFC_WAIT_HELLO,
	PET_WFC_WAIT_JOURNAL
} pet_wfc_state_e;

extern pet_wfc_state_e pet_wfc_state;

#endif
