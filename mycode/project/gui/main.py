import threading
import time
import asyncio

import window
import pet_setup
import club.pet_ble_service as ble
import config
import enums

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


async def ble_update_personality(self):

		print(f"Getting personality for {hex(self.id)}")
		personality = await ble.pet_retrieve_command(self.id, ble.pet_ble_retrieve_personality)

		if personality is None:
			print(f"Couldn't find personality for {hex(self.id)}")
			return
		
		self.mood.affection = personality.affection
		self.mood.happiness = personality.happiness
		self.mood.energy = personality.energy
		self.mood.health = personality.health
		self.mood.interaction = personality.interaction
		self.expression = personality.expression


def mood_rx_ble_comms(pets: list[pet_setup.Pet]):

	while True:
		print(f"Receiving moods")
		try:
			moods: dict[int, ble.PetPEXStatePkt] = asyncio.run(ble.discover_moods())

			for pet_id in moods:
				print(f"Received mood for {hex(pet_id)}")
				for pet in pets:
					if pet_id == pet.id:
						pet.mood.affection = moods[pet_id].affection
						pet.mood.happiness = moods[pet_id].happiness
						pet.mood.energy = moods[pet_id].energy
						pet.mood.health = moods[pet_id].health
						pet.mood.interaction = moods[pet_id].interaction
						state =  pet.mood.happiness / 200

						if state == 0 and pet.mood.happiness < 100:
							pet.expression = enums.Expression.ANGRY.value
						elif state == 0:
							pet.expression = enums.Expression.V_SAD.value
						elif state == 1:
							pet.expression = enums.Expression.SAD.value
						elif state == 2:
							pet.expression = enums.Expression.NEUTRAL.value
						elif state == 3:
							pet.expression = enums.Expression.HAPPY.value
						elif state == 4:
							pet.expression = enums.Expression.V_HAPPY.value
						elif state == 5 and pet.mood.affection == 1000:
							pet.expression = enums.Expression.ENLIGHTENED.value
						else:
							pet.expression = enums.Expression.NEUTRAL.value

			time.sleep(3)

		except Exception:
			print(f"Error trying to retrieve moods")
	

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

	mood_rx_thread = threading.Thread(target=mood_rx_ble_comms, args=(pets,), daemon=True)
	mood_rx_thread.start()

    # Start the Tkinter main loop
	gui_screen.mainloop()
	
