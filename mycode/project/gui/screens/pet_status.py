import tkinter as tk
from tkinter import ttk
from PIL import Image, ImageTk

import pet_setup
import mapping
import config
import club.pet_app_helpers

class PetStatus(tk.Frame):
    def __init__(self, root: tk.Frame, pet: pet_setup.Pet):
        super().__init__(root, bg=config.COLOUR_BACKGROUND)

        self.root = root
        self.pet = pet

        frm_header = tk.Frame(self, bg=config.COLOUR_ACTIVE)
        frm_header.pack(padx=20, pady=20, fill="x", expand=True)
        lbl_header = tk.Label(frm_header, text=f"{pet.name}'s Current Mood", font=("Consolas", 16, "bold"), bg=config.COLOUR_ACTIVE)
        lbl_header.pack(ipady=20)

        self.mood = {
            "Affection": {
                "value": pet.mood.affection,
                "indicators": [],
                "lbl_value": None
            },
            "Happiness": {
                "value": pet.mood.happiness,
                "indicators": [],
                "lbl_value": None
            },
            "Energy": {
                "value": pet.mood.energy,
                "indicators": [],
                "lbl_value": None
            },
            "Health": {
                "value": pet.mood.health,
                "indicators": [],
                "lbl_value": None
            },
            "Interaction": {
                "value": pet.mood.interaction,
                "indicators": [],
                "lbl_value": None
            }
        }

        self.cvs_heatmap = tk.Canvas(self, width=855, height=30, bg="white", highlightthickness=0)
        self.cvs_heatmap.pack()
        self.draw_gradient_bar(855, 30)
        self.cvs_heatmap.create_text(20, 15, fill="#4d5365", text=f"0", font=("Consolas", 14, "bold"))
        self.cvs_heatmap.create_text(820, 15, fill="#4d5365", text=f"1000", font=("Consolas", 14, "bold"))

        self.draw_mood_blocks()

        self.after(2000, self.update_statuses)


    def hex_to_rgb(self, hex_color):
        hex_color = hex_color.lstrip('#')
        return tuple(int(hex_color[i:i+2], 16) for i in (0, 2, 4))

    def rgb_to_hex(self, r, g, b):
        return f'#{r:02x}{g:02x}{b:02x}'
    
    def get_mood_colour(self, value, width=1000, left="#fc7e7e", mid="#f4df8b", right="#8bc58b"):
        rgb_left = self.hex_to_rgb(left)
        rgb_mid = self.hex_to_rgb(mid)
        rgb_right = self.hex_to_rgb(right)

        ratio = value / width
        if ratio > 1:
            ratio = 1
        if ratio < 0.5:
            t = ratio * 2
            r = int(rgb_left[0] + t * (rgb_mid[0] - rgb_left[0]))
            g = int(rgb_left[1] + t * (rgb_mid[1] - rgb_left[1]))
            b = int(rgb_left[2] + t * (rgb_mid[2] - rgb_left[2]))
        else:
            t = (ratio - 0.5) * 2
            r = int(rgb_mid[0] + t * (rgb_right[0] - rgb_mid[0]))
            g = int(rgb_mid[1] + t * (rgb_right[1] - rgb_mid[1]))
            b = int(rgb_mid[2] + t * (rgb_right[2] - rgb_mid[2]))

        return self.rgb_to_hex(r, g, b)

    def draw_gradient_bar(self, width, height, left="#fc7e7e", mid="#f4df8b", right="#8bc58b"):
        for i in range(width):
            colour = self.get_mood_colour(i, width, left, mid, right)
            self.cvs_heatmap.create_line(i, 0, i, height, fill=colour)

    def draw_mood_blocks(self):
        frm_moods = tk.Frame(self, bg="white")
        frm_moods.pack(padx=20, pady=(0, 20), fill="x", expand=True)

        frm_centre = tk.Frame(frm_moods, bg="white")
        frm_centre.pack(anchor="center", padx=10, pady=10)

        moods = list(self.mood.items())

        for i in range(0, len(moods), 3):
            row_frame = tk.Frame(frm_centre, bg="white")
            row_frame.pack(anchor="center", pady=20)

            for name, data in moods[i:i+3]:
                value = data["value"]
                colour = self.get_mood_colour(value)

                frm_colour = tk.Frame(row_frame, width=200, height=200, bg=colour)
                frm_colour.pack(side="left", padx=30)
                frm_colour.pack_propagate(False)

                frm_text = tk.Frame(frm_colour, bg=colour)
                frm_text.place(relx=0.5, rely=0.5, anchor="center")

                lbl_value = tk.Label(frm_text, text=f"{value}", font=("Consolas", 32, "bold"), bg=colour, fg="#4d5365")
                lbl_value.pack()
                lbl_name = tk.Label(frm_text, text=name, font=("Consolas", 14), bg=colour, fg="#4d5365")
                lbl_name.pack()

                self.mood[name]["indicators"].extend([frm_colour, frm_text, lbl_value, lbl_name])
                self.mood[name]["lbl_value"] = lbl_value


    def update_statuses(self):
        self.mood["Affection"]["value"] = self.pet.mood.affection
        self.mood["Happiness"]["value"] = self.pet.mood.happiness
        self.mood["Energy"]["value"] = self.pet.mood.energy
        self.mood["Health"]["value"] = self.pet.mood.health
        self.mood["Interaction"]["value"] = self.pet.mood.interaction

        for name, data in self.mood.items():
            value = data["value"]
            data["lbl_value"].config(text=f"{value}")
            colour = self.get_mood_colour(value)

            for indicator in data["indicators"]:
                indicator.config(bg=colour)
        
        self.after(2000, self.update_statuses)
