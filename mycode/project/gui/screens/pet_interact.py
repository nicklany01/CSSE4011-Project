import tkinter as tk
from tkinter import ttk
from PIL import Image, ImageTk
import copy

import pet_setup
import config
import mapping

class PetInteract(tk.Frame):
    def __init__(self, root: tk.Frame, pet: pet_setup.Pet, other_pets: list[pet_setup.Pet]):
        super().__init__(root, bg=config.COLOUR_BACKGROUND)

        self.root = root
        self.pet = pet
        self.other_pets = other_pets

        self.ent_enviro = {
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
            }
        }

        self.friends = self.pet.friends
        self.enemies = self.pet.enemies

        self.friend_lbl = {}
        self.enemy_lbl = {}

        # Header
        frm_header = tk.Frame(self, bg=config.COLOUR_ACTIVE)
        frm_header.pack(padx=20, pady=20, fill="x", expand=True)
        lbl_header = tk.Label(frm_header, text=f"Interact with {pet.name}!", font=("Consolas", 16, "bold"), bg=config.COLOUR_ACTIVE)
        lbl_header.pack(ipady=20)

        # Environment
        self.frm_enviro = tk.Frame(self, bg=config.COLOUR_ACTIVE, padx=20, pady=20)
        self.frm_enviro.pack(fill="x", expand=True, padx=20)
        tk.Label(self.frm_enviro, text="Current Environment", bg=config.COLOUR_ACTIVE, font=("Consolas", 14, "bold")).pack(anchor="w", pady=(0, 10))
        for lbl in self.ent_enviro:
            row = tk.Frame(self.frm_enviro, bg=config.COLOUR_ACTIVE)
            self.ent_enviro[lbl]["combobox"] = ttk.Combobox(row, values=self.ent_enviro[lbl]["values"], font=("Consolas", 12), state="readonly")
            row.pack(fill="x", pady=5)
            tk.Label(row, text=lbl, bg=config.COLOUR_ACTIVE, font=("Consolas", 12), width=20, anchor="w").pack(side="left")
            self.ent_enviro[lbl]["combobox"].pack(side="left", fill="x", expand=True)

        btn_enviro = tk.Button(self, text="Update Environment", font=("Consolas", 14, "bold"),
                             bg=config.COLOUR_BTN_NORMAL, fg="white", activebackground=config.COLOUR_BTN_ACTIVE, relief="groove",
                             command=self.save_environment, pady=10)
        btn_enviro.pack(fill="x", padx=20, pady=(0, 20))

        self.ent_enviro["Scene"]["combobox"].set(mapping.scene_map[self.pet.scene.scene])
        self.ent_enviro["Time"]["combobox"].set(mapping.time_map[self.pet.scene.time])
        self.ent_enviro["Weather"]["combobox"].set(mapping.weather_map[self.pet.scene.weather])


        # Food
        self.frm_food = tk.Frame(self, bg=config.COLOUR_ACTIVE, padx=20, pady=20)
        self.frm_food.pack(fill="x", expand=True, padx=20)
        tk.Label(self.frm_food, text="Buffet Selection", bg=config.COLOUR_ACTIVE, font=("Consolas", 14, "bold")).pack(anchor="w", pady=(0, 10))
        row = tk.Frame(self.frm_food, bg=config.COLOUR_ACTIVE)
        row.pack(fill="x", pady=5)
        tk.Label(row, text="Food", bg=config.COLOUR_ACTIVE, font=("Consolas", 12), width=20, anchor="w").pack(side="left")
        self.ent_food = ttk.Combobox(row, values=mapping.food_map, font=("Consolas", 12), state="readonly")
        self.ent_food.pack(side="left", fill="x", expand=True)
        self.ent_food.set(mapping.food_map[self.pet.scene.food])

        btn_food = tk.Button(self, text="Feed Pet", font=("Consolas", 14, "bold"),
                             bg=config.COLOUR_BTN_NORMAL, fg="white", activebackground=config.COLOUR_BTN_ACTIVE, relief="groove",
                             command=self.save_food, pady=10)
        btn_food.pack(fill="x", padx=20, pady=(0, 20))


        # Relationships
        self.frm_relate = tk.Frame(self, bg=config.COLOUR_ACTIVE, padx=20, pady=20)
        self.frm_relate.pack(fill="x", expand=True, padx=20)
        tk.Label(self.frm_relate, text="Relationships", bg=config.COLOUR_ACTIVE, font=("Consolas", 14, "bold")).pack(anchor="w", pady=(0, 10))

        self.make_relationship_row("Choose Friends", self.friend_lbl, self.friends, self.enemies)

        
        self.make_relationship_row("Choose Enemies", self.enemy_lbl, self.enemies, self.friends)

        btn_relate = tk.Button(self, text="Be Social", font=("Consolas", 14, "bold"),
                             bg=config.COLOUR_BTN_NORMAL, fg="white", activebackground=config.COLOUR_BTN_ACTIVE, relief="groove",
                             command=self.save_relationships, pady=10)
        btn_relate.pack(fill="x", padx=20, pady=(0, 20))


    def make_relationship_row(self, heading: str, relation_dict: dict, relation_current: list[int], opp_current: list[int]):
        row = tk.Frame(self.frm_relate, bg=config.COLOUR_ACTIVE)
        row.pack(fill="x", pady=5)
        tk.Label(row, text=heading, bg=config.COLOUR_ACTIVE, font=("Consolas", 12)).pack(side="top", anchor="w", pady=5)
        frm_pets = tk.Frame(row, bg=config.COLOUR_INACTIVE)
        frm_pets.pack(fill="x")

        for pet in self.other_pets:
            col = tk.Frame(frm_pets, bg=config.COLOUR_INACTIVE)
            col.pack(side="left", padx="20", pady="20")
            relation_dict[pet.id] = {}
            relation_dict[pet.id]["img"] = ImageTk.PhotoImage(Image.open(f"assets/sprites/sprite_{mapping.sprite_type_map[pet.sprite]}_happy.png").resize((150, 150)))
            relation_dict[pet.id]["lbl_img"] = tk.Label(col, image=relation_dict[pet.id]["img"], bg=(config.COLOUR_BTN_NORMAL if pet.id in relation_current else config.COLOUR_INACTIVE))
            relation_dict[pet.id]["lbl_img"].pack(side="top")
            tk.Label(col, text=pet.name, bg=config.COLOUR_INACTIVE, font=("Consolas", 12)).pack(side="top")

            relation_dict[pet.id]["lbl_img"].bind("<Button-1>", lambda e, p=pet, rd=relation_dict, 
                                                  rc=relation_current, oc=opp_current: 
                                                  self.toggle_relationship(p, rd, rc, oc))

    def toggle_relationship(self, p: pet_setup.Pet, rd: dict, rc: list[int], oc: list[int]):
        if not p.id in rc:
            if not p.id in oc:
                rc.append(p.id)
                rd[p.id]["lbl_img"].configure(bg=config.COLOUR_BTN_NORMAL)
        else:
            rc.remove(p.id)
            rd[p.id]["lbl_img"].configure(bg=config.COLOUR_INACTIVE)
            



    def save_environment(self):
        scene = self.ent_enviro["Scene"]["combobox"].get()
        time = self.ent_enviro["Time"]["combobox"].get()
        weather = self.ent_enviro["Weather"]["combobox"].get()

        self.pet.update_scene(scene, weather, time)
        self.pet.send_state()
        self.pet.save_config()

    def save_food(self):
        food = self.ent_food.get()

        if not food == "None":
            food = "Pizza"

        self.pet.update_food(food)
        self.pet.send_state()
        self.pet.save_config()

    def save_relationships(self):
        self.pet.friends = self.friends
        self.pet.enemies = self.enemies

        self.pet.send_relationships()
        self.pet.save_config()
