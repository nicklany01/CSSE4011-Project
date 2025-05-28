import tkinter as tk
from tkinter import PhotoImage
from PIL import Image, ImageTk

import config
import mapping
import pet_setup


class HomeScreen(tk.Frame):
    def __init__(self, root):
        super().__init__(root)

        self.root = root
        self.pets: list[pet_setup.Pet] = root.pets
        self.images = []

        self.cvs_bg = tk.Canvas(root, width=config.WINDOW_WIDTH, height=config.WINDOW_HEIGHT)
        self.cvs_bg.pack(fill="both", expand=True)

        self.bg_image = PhotoImage(file="assets/home.png")
        self.cvs_bg.create_image(0, 0, image=self.bg_image, anchor="nw")

        self.cvs_bg.create_text(config.WINDOW_WIDTH/2, 200, font=("Consolas", 24), 
                                text="Welcome to")
        self.cvs_bg.create_text(config.WINDOW_WIDTH/2, 250, font=("Consolas", 48), 
                                text="Lucina-Sienna")
        self.cvs_bg.create_text(config.WINDOW_WIDTH/2, 300, font=("Consolas", 12), 
                                text="Mood-Aware Interactive Pet System with Sensor Fusion and Wireless Interactions")
        

        start_x = 350
        spacing = 100
        y_pos = 500

        for i, pet in enumerate(self.pets):
            img = Image.open(f"assets/sprites/sprite_{mapping.sprite_type_map[pet.sprite].lower()}_{mapping.expr_map[pet.expression]}.png").resize((200, 200))
            sprite_img = ImageTk.PhotoImage(img)
            self.images.append(sprite_img)

           
            frm_pet = tk.Frame(root, bg=config.COLOUR_LBL_HIGHLIGHT)
            btn = tk.Button(frm_pet, image=sprite_img, command=lambda p=pet: self.select_pet(p), 
                            borderwidth=0, bg="white")
            btn.pack()

            name_lbl = tk.Label(frm_pet, text=pet.name, font=("Consolas", 14, "bold"), bg=config.COLOUR_LBL_HIGHLIGHT, fg="white")
            name_lbl.pack(pady=(5, 10))

            self.cvs_bg.create_window(start_x + i * (spacing + 150), y_pos, window=frm_pet)

    def select_pet(self, pet: pet_setup.Pet):
        self.root.selected_pet = pet
        self.root.current_frame_num.set(config.FRAME_PET_CONFIG)
