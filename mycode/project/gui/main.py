import threading
import time
import asyncio

import window
import pet_setup
import club.pet_ble_service as ble
import config

def journal_rx_ble_comms(pets: list[pet_setup.Pet]):

	while True:
		for pet in pets:
			print(f"Receiving for: {hex(pet.id)}")

			if pet.id in config.TEST_IDS:
				time.sleep(1)

			try:
				asyncio.run(pet.ble_update_journal(pets))
			except Exception:
				print(f"Error trying to retrieve journal for {hex(pet.id)}")

			# asyncio.run(pet.ble_update_personality())

def mood_rx_ble_comms(pets: list[pet_setup.Pet]):

	while True:
		for pet in pets:
			print(f"Receiving for: {hex(pet.id)}")
			time.sleep(3)

			try:
				asyncio.run(pet.ble_update_personality())
			except Exception:
				print(f"Error trying to retrieve mood for {hex(pet.id)}")
	

if __name__ == '__main__':

	pets_in_area = asyncio.run(ble.pet_ble_discover_pets())
	print(f"Found these pets: {pets_in_area}")

	pet_ids = []

	for pet in pets_in_area:
		if len(pet_ids) == 3:
			break
		pet_ids.append(pet)

	num_default = 3 - len(pet_ids)
	for i in range(0, num_default):
		if i == 0:
			pet_ids.append(66)
		elif i == 1:
			pet_ids.append(77)
		elif i == 2:
			pet_ids.append(88)

	pets = []
	for pet_id in pet_ids:
		pets.append(pet_setup.Pet(f"data/pet_{pet_id}.json"))
	
	gui_screen = window.GUIWindow(pets)
	
	# Async BLE comms
	journal_rx_thread = threading.Thread(target=journal_rx_ble_comms, args=(pets,), daemon=True)
	journal_rx_thread.start()

	# mood_rx_thread = threading.Thread(target=mood_rx_ble_comms, args=(pets,), daemon=True)
	# mood_rx_thread.start()

    # Start the Tkinter main loop
	gui_screen.mainloop()
	
