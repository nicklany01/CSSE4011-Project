import tkinter as tk
from tkinter import ttk
from PIL import Image, ImageTk

import pet_setup
import mapping
import config

class PetConfig(tk.Frame):
    def __init__(self, root: tk.Frame, container: tk.Frame, pet: pet_setup.Pet):
        super().__init__(root, bg=config.COLOUR_BACKGROUND)

        self.root = root
        self.container = container
        self.pet = pet

        # Configurable values
        self.pet_name = tk.StringVar(value=pet.name)
        self.ent_sprite = None

        self.ent_threshold = {
            "Energy Threshold": None,
            "Health Threshold": None,
            "Interaction Threshold": None
        }

        self.ent_faves = {
            "Scene": {
                "combobox": None,
                "values": mapping.scene_map
            },
            "Time": {
                "combobox": None,
                "values": mapping.time_map
            },
            "Weather": {
                "combobox": None,
                "values": mapping.weather_map
            },
            "Food": {
                "combobox": None,
                "values": mapping.food_map
            },
            "Drink": {
                "combobox": None,
                "values": mapping.drink_map
            }
        }

        # Button
        btn_save = tk.Button(self, text="Send and Save Changes", font=("Consolas", 14, "bold"),
                             bg="#70CB73", fg="white", activebackground="#94f599", relief="groove",
                             command=self.save_changes, pady=20)
        btn_save.pack(fill="x", padx=20, pady=20)

        # Personal Details
        self.frm_personal = tk.Frame(self, bg=config.COLOUR_ACTIVE, padx=20, pady=20)
        self.frm_personal.pack(fill="x", expand=True, pady=(0, 20), padx=20)
        tk.Label(self.frm_personal, text="Personal Details", bg=config.COLOUR_ACTIVE, font=("Consolas", 14, "bold")).pack(anchor="w", pady=(0, 10))

        frm_id = tk.Frame(self.frm_personal, bg=config.COLOUR_ACTIVE)
        tk.Label(frm_id, text="ID Number", bg=config.COLOUR_ACTIVE, font=("Consolas", 12), width=20, anchor="w").pack(side="left")
        self.ent_name = tk.Label(frm_id, text=f"{pet.id}", bg=config.COLOUR_ACTIVE, font=("Consolas", 12)).pack(side="left")
        frm_id.pack(fill="x", pady=5)
        
        frm_name = tk.Frame(self.frm_personal, bg=config.COLOUR_ACTIVE)
        tk.Label(frm_name, text="Name", bg=config.COLOUR_ACTIVE, font=("Consolas", 12), width=20, anchor="w").pack(side="left")
        self.ent_name = tk.Entry(frm_name, textvariable=self.pet_name, font=("Consolas", 12)).pack(side="left", fill="x", expand=True)
        frm_name.pack(fill="x", pady=5)

        frm_sprite = tk.Frame(self.frm_personal, bg=config.COLOUR_ACTIVE)
        tk.Label(frm_sprite, text="Sprite", bg=config.COLOUR_ACTIVE, font=("Consolas", 12), width=20, anchor="w").pack(side="left")
        self.ent_sprite = ttk.Combobox(frm_sprite, values=mapping.sprite_type_map, font=("Consolas", 12), state="readonly")
        frm_sprite.pack(fill="x", pady=5)
        self.ent_sprite.pack(side="left", fill="x", expand=True)
        self.ent_sprite.set(mapping.sprite_type_map[self.pet.sprite])


        # Thresholds
        self.frm_threshold = tk.Frame(self, bg=config.COLOUR_ACTIVE, padx=20, pady=20)
        self.frm_threshold.pack(fill="x", expand=True, pady=(0, 20), padx=20)
        tk.Label(self.frm_threshold, text="Thresholds", bg=config.COLOUR_ACTIVE, font=("Consolas", 14, "bold")).pack(anchor="w", pady=(0, 10))
        for lbl in self.ent_threshold:
            row = tk.Frame(self.frm_threshold, bg=config.COLOUR_ACTIVE)
            self.ent_threshold[lbl] = tk.Scale(row, from_=0, to=1000, orient="horizontal",
                                               resolution=10, length=400, tickinterval=250,
                                               troughcolor="white", bg=config.COLOUR_INACTIVE,
                                               highlightthickness=0)
            row.pack(fill="x", pady=5)
            tk.Label(row, text=lbl, bg=config.COLOUR_ACTIVE, font=("Consolas", 12)).pack(side="top", anchor="w", pady=5)
            self.ent_threshold[lbl].pack(side="top", anchor="w", fill="x", expand=True)

        self.ent_threshold["Energy Threshold"].set(self.pet.energy_threshold)
        self.ent_threshold["Health Threshold"].set(self.pet.health_threshold)
        self.ent_threshold["Interaction Threshold"].set(self.pet.interation_threshold)


        # Favourites
        self.frm_faves = tk.Frame(self, bg=config.COLOUR_ACTIVE, padx=20, pady=20)
        self.frm_faves.pack(fill="x", expand=True, pady=(0, 20), padx=20)
        tk.Label(self.frm_faves, text="Favourites", bg=config.COLOUR_ACTIVE, font=("Consolas", 14, "bold")).pack(anchor="w", pady=(0, 10))
        for lbl in self.ent_faves:
            row = tk.Frame(self.frm_faves, bg=config.COLOUR_ACTIVE)
            self.ent_faves[lbl]["combobox"] = ttk.Combobox(row, values=self.ent_faves[lbl]["values"], font=("Consolas", 12), state="readonly")
            row.pack(fill="x", pady=5)
            tk.Label(row, text=lbl, bg=config.COLOUR_ACTIVE, font=("Consolas", 12), width=20, anchor="w").pack(side="left")
            self.ent_faves[lbl]["combobox"].pack(side="left", fill="x", expand=True)

        self.ent_faves["Scene"]["combobox"].set(mapping.scene_map[self.pet.favourites.scene])
        self.ent_faves["Time"]["combobox"].set(mapping.time_map[self.pet.favourites.time])
        self.ent_faves["Weather"]["combobox"].set(mapping.weather_map[self.pet.favourites.weather])
        self.ent_faves["Food"]["combobox"].set(mapping.food_map[self.pet.favourites.food])
        self.ent_faves["Drink"]["combobox"].set(mapping.drink_map[self.pet.favourites.drink])

    
    def save_changes(self):
        name = self.pet_name.get()
        sprite = self.ent_sprite.get()
        energy_threshold = self.ent_threshold["Energy Threshold"].get()
        health_threshold = self.ent_threshold["Health Threshold"].get()
        interaction_threshold = self.ent_threshold["Interaction Threshold"].get()
        fav_scene = self.ent_faves["Scene"]["combobox"].get()
        fav_time = self.ent_faves["Time"]["combobox"].get()
        fav_weather = self.ent_faves["Weather"]["combobox"].get()
        fav_food = self.ent_faves["Food"]["combobox"].get()
        fav_drink = self.ent_faves["Drink"]["combobox"].get()

        self.pet.update_config(name, sprite, energy_threshold, health_threshold, interaction_threshold,
                               fav_scene, fav_time, fav_weather, fav_food, fav_drink)
        
        self.container.img_banner = ImageTk.PhotoImage(Image.open(f"assets/{mapping.sprite_file_map[self.pet.sprite]}").resize((110, 150)))
        self.container.cvs_bg.create_image(500, 15, image=self.container.img_banner, anchor="ne")
        
        self.pet.save_config()