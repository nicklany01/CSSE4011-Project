import json

import config
import mapping
import club.pet_app_helpers
from enums import Scene, Time, Weather, Food, Drink, Sprite

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
    def __init__(self, scene: int=Scene.MAIN_SCENE_MEADOW, time: int=Time.MOD_TIME_MORNING, 
				 weather: int=Weather.MOD_WEATHER_SUNNY, food: int=Food.FOOD_NONE, 
				 drink: int=Drink.DRINK_NONE):
        self.scene = scene
        self.time = time
        self.weather = weather
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

			self.id = pet_config["id"]
			self.name = pet_config["name"]
			self.sprite = pet_config["sprite"]
			self.energy_threshold = pet_config["energy_threshold"]
			self.health_threshold = pet_config["health_threshold"]
			self.interation_threshold = pet_config["interation_threshold"]

			mood = pet_config["mood"]
			self.mood = PetMood(mood["affection"], mood["happiness"], mood["energy"], mood["health"], mood["interaction"])

			faves = pet_config["favourites"]
			self.favourites = PetFavourites(faves["scene"], faves["time"], faves["weather"], faves["food"], faves["drink"])

			self.friends = []
			for friend in pet_config["friends"]:
				self.friends.append(friend)

			self.journal = club.pet_app_helpers.PetJournal()
			for time, event in pet_config["journal"]:
				journal_entry = club.pet_app_helpers.PetJournalEntry(time, event)
				self.journal.add(journal_entry)

		except Exception as e:
			print(f"Error: Could not properly load pet from '{file_path}': {e}")
			self.reset_pet()

	def reset_pet(self):
		self.id = 0
		self.name = ""
		self.sprite = Sprite.SPRITE_ZERO

		self.energy_threshold = 500
		self.health_threshold = 500
		self.interation_threshold = 500

		self.mood = PetMood()
		self.favourites = PetFavourites()
		self.friends = []
		self.journal = club.pet_app_helpers.PetJournal()

	def update_config(self, name: str, sprite: str, energy_threshold: int, health_threshold: int,
				   interaction_threshold: int, fav_scene: str, fav_time: str, fav_weather: str,
				   fav_food: str, fav_drink: str):
		self.name = name
		self.sprite = mapping.sprite_type_map.index(sprite)
		self.energy_threshold = energy_threshold
		self.health_threshold = health_threshold
		self.interation_threshold = interaction_threshold
		self.favourites.scene = mapping.scene_map.index(fav_scene)
		self.favourites.time = mapping.time_map.index(fav_time)
		self.favourites.weather = mapping.weather_map.index(fav_weather)
		self.favourites.food = mapping.food_map.index(fav_food)
		self.favourites.drink = mapping.drink_map.index(fav_drink)

	def save_config(self):
		pet_config = {}
		pet_config["id"] = self.id
		pet_config["name"] = self.name
		pet_config["sprite"] = self.sprite
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

		pet_config["friends"] = []
		for friend in self.friends:
			pet_config["friends"].append(friend)

		pet_config["journal"] = []
		for journal_entry in self.journal.entries:
			if journal_entry is None:
				continue
			pet_config["journal"].append([journal_entry.timestamp, journal_entry.event])

		with open(f"data/pet_{self.id}.json", "w") as f:
			json.dump(pet_config, f, indent=4)