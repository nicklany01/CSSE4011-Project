import json
import asyncio
import threading

import config
import mapping
import club.pet_app_helpers
from enums import Scene, Time, Weather, Food, Drink, Sprite, Expression

import club.pet_ble_service as ble

class PetMood():
    def __init__(self, affection: int=config.DEFAULT_AFFECTION, happiness: int=config.DEFAULT_HAPPINESS, 
				 energy: int=config.DEFAULT_ENERGY, health: int=config.DEFAULT_HEALTH, 
				 interaction: int=config.DEFAULT_INTERACTION):
        self.affection = affection
        self.happiness = happiness
        self.energy = energy
        self.health = health
        self.interaction = interaction


class PetFavourites():
    def __init__(self, scene: int=Scene.MAIN_SCENE_MEADOW.value, time: int=Time.MOD_TIME_MORNING.value, 
				 weather: int=Weather.MOD_WEATHER_SUNNY.value, food: int=Food.FOOD_NONE.value, 
				 drink: int=Drink.DRINK_NONE.value):
        self.scene = scene
        self.time = time
        self.weather = weather
        self.food = food
        self.drink = drink

# class PetAttributes():
# 	def __init__(self, charisma: int=1, confidence: int=1, kindness: int=1, patience: int=1, 
# 				 lazy: int=1, rude: int=1, gaslight: int=1, greedy: int=1):
# 		self.charisma = charisma
# 		self.confidence = confidence
# 		self.kindness = kindness
# 		self.patience = patience

# 		self.lazy = lazy
# 		self.rude = rude
# 		self.gaslight = gaslight
# 		self.greedy = greedy

class PetScene():
	def __init__(self, scene: int=Scene.MAIN_SCENE_MEADOW.value, weather: int=Weather.MOD_WEATHER_SUNNY.value, 
			  time: int=Time.MOD_TIME_MORNING.value, food: int=Food.FOOD_NONE.value, 
			  drink: int=Drink.DRINK_NONE.value):
		self.scene = scene
		self.weather = weather
		self.time = time

		self.food = food
		self.drink = drink


class Pet():
	def __init__(self, file_path: str=""):
		if file_path == "":
			self.reset_pet()
			return

		try:
			pet_config = {}
			with open(file_path) as f:
				pet_config = json.load(f)

			self.id: int = pet_config["id"]
			self.name: str = pet_config["name"]
			self.sprite: int = pet_config["sprite"]
			self.expression: int = pet_config["expression"]

			self.energy_threshold: int = pet_config["energy_threshold"]
			self.health_threshold: int = pet_config["health_threshold"]
			self.interation_threshold: int = pet_config["interation_threshold"]

			mood = pet_config["mood"]
			self.mood: PetMood = PetMood(mood["affection"], mood["happiness"], mood["energy"], 
								mood["health"], mood["interaction"])

			faves = pet_config["favourites"]
			self.favourites: PetFavourites = PetFavourites(faves["scene"], faves["time"], 
												  faves["weather"], faves["food"], faves["drink"])

			# attributes = pet_config["attributes"]
			# self.attributes: PetAttributes = PetAttributes(attributes["charisma"], attributes["confidence"], 
			# 									  attributes["kindness"], attributes["patience"], attributes["lazy"],
			# 									  attributes["rude"], attributes["gaslight"], attributes["greedy"])

			scene = pet_config["scene"]
			self.scene: PetScene = PetScene(scene["scene"], scene["weather"], scene["time"], 
								   scene["food"], scene["drink"])

			self.friends: list[int] = []
			for friend in pet_config["friends"]:
				self.friends.append(friend)

			self.enemies: list[int] = []
			for enemy in pet_config["enemies"]:
				self.enemies.append(enemy)

			self.journal: club.pet_app_helpers.PetJournal = club.pet_app_helpers.PetJournal()
			# journal_entry = club.pet_app_helpers.PetJournalEntry()
			# for time, event in pet_config["journal"]:
			# 	journal_entry.timestamp = time
			# 	journal_entry.event = event
			# 	self.journal.add(journal_entry)

		except Exception as e:
			print(f"Error: Could not properly load pet from '{file_path}': {e}")
			self.reset_pet()

	def reset_pet(self):
		self.id: int = 0
		self.name: str = ""
		self.sprite: int = Sprite.SPRITE_ZERO.value
		self.expression: int = Expression.NEUTRAL.value

		self.energy_threshold: int = 500
		self.health_threshold: int = 500
		self.interation_threshold: int = 500

		self.mood: PetMood = PetMood()
		self.favourites: PetFavourites = PetFavourites()
		# self.attributes: PetAttributes = PetAttributes()
		self.scene: PetScene = PetScene()
		self.friends: list[int] = []
		self.enemies: list[int] = []
		self.journal: club.pet_app_helpers.PetJournal = club.pet_app_helpers.PetJournal()

	def update_config(self, name: str, id: str, sprite: str, energy_threshold: int, health_threshold: int,
				   interaction_threshold: int, fav_scene: str, fav_time: str, fav_weather: str,
				   fav_food: str, fav_drink: str, charisma: int=0, confidence: int=0, kindness: int=0, 
				   patience: int=0, lazy: int=0, rude: int=0, gaslight: int=0, greedy: int=0):
		self.name = name

		if id == "":
			self.id = 0
		else:
			self.id = int(id)
		self.sprite = mapping.sprite_type_map.index(sprite)
		self.energy_threshold = energy_threshold
		self.health_threshold = health_threshold
		self.interation_threshold = interaction_threshold

		self.favourites.scene = mapping.scene_map.index(fav_scene)
		self.favourites.time = mapping.time_map.index(fav_time)
		self.favourites.weather = mapping.weather_map.index(fav_weather)
		self.favourites.food = mapping.food_map.index(fav_food)
		self.favourites.drink = mapping.drink_map.index(fav_drink)

		# self.attributes.charisma = charisma
		# self.attributes.confidence = confidence
		# self.attributes.kindness = kindness
		# self.attributes.patience = patience
		# self.attributes.lazy = lazy
		# self.attributes.rude = rude
		# self.attributes.gaslight = gaslight
		# self.attributes.greedy = greedy

	def update_scene(self, scene: str, weather: str, time: str):
		self.scene.scene = mapping.scene_map.index(scene)
		self.scene.time = mapping.time_map.index(time)
		self.scene.weather = mapping.weather_map.index(weather)
		self.scene.food = Food.FOOD_NONE.value
		self.scene.drink = Drink.DRINK_NONE.value

	
	def update_food(self, food: str="Pizza"):
		self.scene.food = mapping.food_map.index(food)
		self.scene.drink = Drink.DRINK_NONE.value

	
	def update_drink(self, drink: str="Cola"):
		self.scene.food = Food.FOOD_NONE.value
		self.scene.drink = mapping.drink_map.index(drink)


	def save_config(self):
		pet_config = {}
		pet_config["id"] = self.id
		pet_config["name"] = self.name
		pet_config["sprite"] = self.sprite
		pet_config["expression"] = self.expression

		pet_config["energy_threshold"] = self.energy_threshold
		pet_config["health_threshold"] = self.health_threshold
		pet_config["interation_threshold"] = self.interation_threshold

		pet_config["mood"] = {}
		pet_config["mood"]["affection"] = self.mood.affection
		pet_config["mood"]["happiness"] = self.mood.happiness
		pet_config["mood"]["energy"] = self.mood.energy
		pet_config["mood"]["health"] = self.mood.health
		pet_config["mood"]["interaction"] = self.mood.interaction

		pet_config["favourites"] = {}
		pet_config["favourites"]["scene"] = self.favourites.scene
		pet_config["favourites"]["time"] = self.favourites.time
		pet_config["favourites"]["weather"] = self.favourites.weather
		pet_config["favourites"]["food"] = self.favourites.food
		pet_config["favourites"]["drink"] = self.favourites.drink

		pet_config["scene"] = {}
		pet_config["scene"]["scene"] = self.scene.scene
		pet_config["scene"]["time"] = self.scene.time
		pet_config["scene"]["weather"] = self.scene.weather
		pet_config["scene"]["food"] = self.scene.food
		pet_config["scene"]["drink"] = self.scene.drink

		# pet_config["attributes"] = {}
		# pet_config["attributes"]["charisma"] = self.attributes.charisma
		# pet_config["attributes"]["confidence"] = self.attributes.confidence
		# pet_config["attributes"]["kindness"] = self.attributes.kindness
		# pet_config["attributes"]["patience"] = self.attributes.patience
		# pet_config["attributes"]["lazy"] = self.attributes.lazy
		# pet_config["attributes"]["rude"] = self.attributes.rude
		# pet_config["attributes"]["gaslight"] = self.attributes.gaslight
		# pet_config["attributes"]["greedy"] = self.attributes.greedy

		pet_config["friends"] = []
		for friend in self.friends:
			pet_config["friends"].append(friend)

		pet_config["enemies"] = []
		for enemy in self.enemies:
			pet_config["enemies"].append(enemy)

		pet_config["journal"] = []
		for journal_entry in self.journal.entries:
			if journal_entry is None:
				continue
			pet_config["journal"].append([journal_entry.timestamp, journal_entry.event])

		with open(f"data/pet_{self.id}.json", "w") as f:
			json.dump(pet_config, f, indent=4)


	async def ble_update_journal(self, pets: list):
		await ble.pet_ble_init()

		print(f"Getting journals for {hex(self.id)}")
		pet_journals: list[ble.PetJournal] = await ble.pet_retrieve_command(self.id, ble.pet_ble_retrieve_journal)

		if pet_journals is None:
			print(f"Couldn't find journals for {hex(self.id)}")
			return

		for pet_id in pet_journals:
			print(f"JOURNAL OF PET {hex(pet_id)}")
			print(pet_journals[pet_id])
			for pet in pets:
				if pet.id == pet_id:
					pet.journal = pet_journals[pet_id]
					break
	

	async def ble_update_personality(self):
		await ble.pet_ble_init()

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

	
	def send_personality(self):
		print("Sending pet personality...")
		ppy_pkt = ble.PetPPYPersonalityPkt()
		ppy_pkt.randomize()

		ppy_pkt.sprite = self.sprite
		ppy_pkt.fav_scene = self.favourites.scene
		ppy_pkt.fav_weather = self.favourites.weather
		ppy_pkt.fav_time = self.favourites.time
		ppy_pkt.fav_temp = 0
		ppy_pkt.fav_food = self.favourites.food
		ppy_pkt.fav_drink = self.favourites.drink

		tx_thread = threading.Thread(target=self.tx_send_personality, args=(ppy_pkt,), daemon=True)
		tx_thread.start()


	def tx_send_personality(self, ppy_pkt):
		asyncio.run(ble.pet_ble_set_personality(self.id, ppy_pkt))

	
	def send_state(self):
		print("Sending pet state...")
		state_pkt = ble.PetPEXStatePkt()

		state_pkt.scene = self.scene.scene
		state_pkt.scene_weather = self.scene.weather
		state_pkt.scene_mood = 0
		state_pkt.scene_time = self.scene.time
		state_pkt.scene_temp = 0

		self.held_food = self.scene.food
		self.held_drink = self.scene.drink

		tx_thread = threading.Thread(target=self.tx_send_state, args=(state_pkt,), daemon=True)
		tx_thread.start()

	def tx_send_state(self, state_pkt):
		asyncio.run(ble.pet_ble_set_state(self.id, state_pkt))


	def send_relationships(self, current_friends: list[int], new_friends: list[int], 
						current_enemies: list[int], new_enemies: list[int]):
		print("Updating relationships...")

		add_friends = []
		remove_friends = []

		add_enemies = []
		remove_enemies = []

		for new_friend in new_friends:
			if not new_friend in current_friends:
				add_friends.append(new_friend)
		
		for current_friend in current_friends:
			if not current_friend in new_friends:
				remove_friends.append(current_friend)

		for new_enemy in new_enemies:
			if not new_enemy in current_enemies:
				add_enemies.append(new_enemy)
		
		for current_enemy in current_enemies:
			if not current_enemy in new_enemies:
				remove_enemies.append(current_enemy)

		relation_pkts = []
		for friend in add_friends:
			print("Adding friend")
			relationship_pkt = ble.PetWFCDemoCmdPkt()
			relationship_pkt.cmd_id = ble.PET_WFC_DEMO_CMDS.ADD_FRIEND
			relationship_pkt.cmd_arg = friend
			relation_pkts.append(relationship_pkt)

		for friend in remove_friends:
			print("Removing friend")
			relationship_pkt = ble.PetWFCDemoCmdPkt()
			relationship_pkt.cmd_id = ble.PET_WFC_DEMO_CMDS.REM_FRIEND
			relationship_pkt.cmd_arg = friend
			relation_pkts.append(relationship_pkt)

		for enemy in add_enemies:
			print("Adding ememy")
			relationship_pkt = ble.PetWFCDemoCmdPkt()
			relationship_pkt.cmd_id = ble.PET_WFC_DEMO_CMDS.ADD_ENEMY
			relationship_pkt.cmd_arg = enemy
			relation_pkts.append(relationship_pkt)

		for enemy in remove_enemies:
			print("Removing enemy")
			relationship_pkt = ble.PetWFCDemoCmdPkt()
			relationship_pkt.cmd_id = ble.PET_WFC_DEMO_CMDS.REM_ENEMY
			relationship_pkt.cmd_arg = enemy
			relation_pkts.append(relationship_pkt)

		tx_thread = threading.Thread(target=self.tx_send_relationships, args=(relation_pkts,), daemon=True)
		tx_thread.start()

	def tx_send_relationships(self, pkts):
		asyncio.run(ble.pet_ble_update_relationship(self.id, pkts))

