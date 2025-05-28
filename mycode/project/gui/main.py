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
		
		time.sleep(5)
	

if __name__ == '__main__':
	pet0 = pet_setup.Pet("data/pet_0.json")
	pet1 = pet_setup.Pet("data/pet_1.json")
	pet2 = pet_setup.Pet("data/pet_2.json")
	
	gui_screen = window.GUIWindow([pet0, pet1, pet2])
	
	# Async BLE comms
	rx_thread = threading.Thread(target=rx_ble_comms, args=([pet0, pet1, pet2],), daemon=True)
	rx_thread.start()

    # Start the Tkinter main loop
	gui_screen.mainloop()
	
