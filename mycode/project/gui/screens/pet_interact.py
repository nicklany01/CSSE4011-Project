import tkinter as tk

import pet_setup
import config
import club.pet_app_helpers

class PetInteract(tk.Frame):
    def __init__(self, root: tk.Frame, pet: pet_setup.Pet):
        super().__init__(root, bg=config.COLOUR_BACKGROUND)

        self.root = root
        self.pet = pet

        # Header
        frm_header = tk.Frame(self, bg=config.COLOUR_ACTIVE)
        frm_header.pack(padx=20, pady=20, fill="x", expand=True)
        lbl_header = tk.Label(frm_header, text=f"Interact with {pet.name}!", font=("Consolas", 16, "bold"), bg=config.COLOUR_ACTIVE)
        lbl_header.pack(ipady=20)