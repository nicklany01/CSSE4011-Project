import json

import config
from enums import Scene, Time, Weather, Food, Drink, Sprite

class PetMood():
    def __init__(self):
        self.affection = config.DEFAULT_AFFECTION
        self.happiness = config.DEFAULT_HAPPINESS
        self.energy = config.DEFAULT_ENERGY
        self.health = config.DEFAULT_HEALTH
        self.interaction = config.DEFAULT_INTERACTION

    def __init__(self, affection: int, happiness: int, energy: int, health: int, interaction: int):
        self.affection = affection
        self.happiness = happiness
        self.energy = energy
        self.health = health
        self.interaction = interaction


class PetFavourites():
    def __init__(self):
        self.scene = Scene.MAIN_SCENE_MEADOW
        self.time = Time.MOD_TIME_MORNING
        self.weather = Weather.MOD_WEATHER_SUNNY
        self.food = Food.FOOD_NONE
        self.drink = Drink.DRINK_NONE

    def __init__(self, scene: int, time: int, weather: int, food: int, drink: int):
        self.scene = scene
        self.time = time
        self.weather = weather
        self.food = food
        self.drink = drink


class Pet():
	def __init__(self):
		self.reset_pet()

	def __init__(self, file_path: str):
		
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

			self.journal = []
			for entry in pet_config["journal"]:
				self.journal.append(entry)

		except Exception as e:
			print(f"Error: Could not properly load pet from '{file_path}'")
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
		self.journal = []

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
		for entry in self.journal:
			pet_config["journal"].append(entry)

		with open(f"pet_{self.id}.json", "w") as f:
			json.dump(pet_config, f, indent=4)