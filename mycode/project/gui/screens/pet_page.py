import tkinter as tk
from PIL import Image, ImageTk

import pet_setup
import mapping
import config

COLOUR_ACTIVE = "#DCD0B7"
COLOUR_INACTIVE = "#BDAE91"
COLOUR_BACKGROUND = "#f5eedb"

class PetPage(tk.Frame):
    def __init__(self, root: tk.Tk):
        super().__init__(root)

        self.root = root
        self.pet: pet_setup.Pet = root.selected_pet

        # Sidebar
        self.frm_sidebar = tk.Frame(self, bg=COLOUR_INACTIVE, width=200)
        self.frm_sidebar.pack(side=tk.LEFT, fill=tk.Y)

        self.tabs = {
            "Home": {
                "command": self.show_home,
                "img": ImageTk.PhotoImage(Image.open("assets/icon_home.png").resize((50, 50))),
                "lbl": None,
                "frame": None
            },
            "Status": {
                "command": self.show_status,
                "img": ImageTk.PhotoImage(Image.open("assets/icon_status.png").resize((50, 50))),
                "lbl": None,
                "frame": None
            },
            "Journal": {
                "command": self.show_journal,
                "img": ImageTk.PhotoImage(Image.open("assets/icon_journal.png").resize((50, 50))),
                "lbl": None,
                "frame": None
            },
            "Settings": {
                "command": self.show_settings,
                "img": ImageTk.PhotoImage(Image.open("assets/icon_settings.png").resize((50, 50))),
                "lbl": None,
                "frame": None
            }
        }

        for tab_name in self.tabs:
            frm_tab = tk.Frame(self.frm_sidebar, bg=COLOUR_INACTIVE, padx=10, pady=5)
            frm_tab.pack(fill=tk.X)

            lbl_icon = tk.Label(frm_tab, image=self.tabs[tab_name]["img"], bg=COLOUR_INACTIVE)
            lbl_icon.pack(side=tk.LEFT)

            self.tabs[tab_name]["lbl"] = tk.Label(frm_tab, text=tab_name, bg=COLOUR_INACTIVE, font=("Consolas", 16))
            self.tabs[tab_name]["lbl"].pack(side=tk.LEFT, padx=(10, 100))

            # Make tab row clickable
            def bind_all(widget, tabname):
                widget.bind("<Button-1>", lambda e: self.select_tab(tabname))
                for child in widget.winfo_children():
                    child.bind("<Button-1>", lambda e: self.select_tab(tabname))

            bind_all(frm_tab, tab_name)
            self.tabs[tab_name]["frame"] = frm_tab

        # Content side
        self.frm_content = tk.Frame(self, bg="white")
        self.frm_content.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

        # Banner
        self.frm_banner = tk.Frame(self.frm_content, bg=COLOUR_BACKGROUND, height=180)
        self.frm_banner.pack(fill=tk.X)
        self.cvs_bg = tk.Canvas(self.frm_banner, height=180)
        self.cvs_bg.pack(fill="both", expand=True)

        self.bg_image = ImageTk.PhotoImage(file="assets/home.png")
        self.cvs_bg.create_image(0, 0, image=self.bg_image, anchor="nw")

        self.img_banner = ImageTk.PhotoImage(Image.open(f"assets/{mapping.sprite_map[self.pet.sprite]}").resize((110, 150)))
        self.cvs_bg.create_image(500, 15, image=self.img_banner, anchor="ne")

        # Screen
        self.frm_screen = tk.Frame(self.frm_content, bg=COLOUR_BACKGROUND)
        self.frm_screen.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

        self.select_tab("Status")


    def select_tab(self, name: str):
        for tab_name in self.tabs:
            if tab_name == name:
                self.tabs[tab_name]["frame"].configure(bg=COLOUR_ACTIVE)
                self.tabs[tab_name]["lbl"].configure(bg=COLOUR_ACTIVE)
            else:
                self.tabs[tab_name]["frame"].configure(bg=COLOUR_INACTIVE)
                self.tabs[tab_name]["lbl"].configure(bg=COLOUR_INACTIVE)

        self.clear_tab()
        self.tabs[name]["command"]()

    def clear_tab(self):
        for widget in self.frm_screen.winfo_children():
            widget.destroy()

    def show_home(self):
        self.root.selected_pet = None
        self.root.current_frame_num.set(config.FRAME_HOME)

    def show_status(self):
        tk.Label(self.frm_screen, text=f"{self.pet.name}'s Status", font=("Arial", 20), bg="white").pack(pady=20)
        mood = self.pet.mood
        stats = {
            "Affection": mood.affection,
            "Happiness": mood.happiness,
            "Energy": mood.energy,
            "Health": mood.health,
            "Interaction": mood.interaction,
        }
        for k, v in stats.items():
            tk.Label(self.frm_screen, text=f"{k}: {v}", font=("Arial", 14), bg="white").pack(pady=5)

    def show_journal(self):
        tk.Label(self.frm_screen, text="Journal Entries", font=("Arial", 20), bg="white").pack(pady=20)
        for entry in self.pet.journal:
            tk.Label(self.frm_screen, text=f"• {entry}", font=("Arial", 12), anchor="w", bg="white").pack(fill=tk.X, padx=20, pady=2)

    def show_settings(self):
        tk.Label(self.frm_screen, text="Settings", font=("Arial", 20), bg="white").pack(pady=20)
        tk.Label(self.frm_screen, text="(Settings UI coming soon...)", font=("Arial", 12), bg="white").pack()