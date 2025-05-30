import threading
import time
import asyncio

import window
import pet_setup
import club.pet_ble_service as ble


def rx_ble_comms(pets: list[pet_setup.Pet]):

	while True:
		for pet in pets:
			asyncio.run(pet.ble_update_journal(pets))
			# asyncio.run(pet.ble_update_personality())
		
		time.sleep(3)
	

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
		pet_ids.append(i)

	pets = []
	for pet_id in pet_ids:
		pets.append(pet_setup.Pet(f"data/pet_{pet_id}.json"))
	
	gui_screen = window.GUIWindow(pets)
	
	# Async BLE comms
	rx_thread = threading.Thread(target=rx_ble_comms, args=(pets,), daemon=True)
	rx_thread.start()

    # Start the Tkinter main loop
	gui_screen.mainloop()
	
