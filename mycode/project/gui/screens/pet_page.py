import tkinter as tk
from PIL import Image, ImageTk

import pet_setup
import mapping
import config

import screens.pet_config
import screens.pet_interact
import screens.pet_journal
import screens.pet_status

class PetPage(tk.Frame):
    def __init__(self, root: tk.Tk):
        super().__init__(root)

        self.root = root
        self.pet: pet_setup.Pet = root.selected_pet

        # Sidebar
        self.frm_sidebar = tk.Frame(self, bg=config.COLOUR_INACTIVE, width=200)
        self.frm_sidebar.pack(side="left", fill="y")

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
            "Interact": {
                "command": self.show_interact,
                "img": ImageTk.PhotoImage(Image.open("assets/icon_interact.png").resize((50, 50))),
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
            frm_tab = tk.Frame(self.frm_sidebar, bg=config.COLOUR_INACTIVE, padx=10, pady=5)
            frm_tab.pack(fill="x")

            lbl_icon = tk.Label(frm_tab, image=self.tabs[tab_name]["img"], bg=config.COLOUR_INACTIVE)
            lbl_icon.pack(side="left")

            self.tabs[tab_name]["lbl"] = tk.Label(frm_tab, text=tab_name, bg=config.COLOUR_INACTIVE, font=("Consolas", 16))
            self.tabs[tab_name]["lbl"].pack(side="left", padx=(10, 100))

            # Make tab row clickable
            def bind_all(widget, tabname):
                widget.bind("<Button-1>", lambda e: self.select_tab(tabname))
                for child in widget.winfo_children():
                    child.bind("<Button-1>", lambda e: self.select_tab(tabname))

            bind_all(frm_tab, tab_name)
            self.tabs[tab_name]["frame"] = frm_tab

        # Content side
        self.frm_content = tk.Frame(self, bg="white")
        self.frm_content.pack(side="left", fill="both", expand=True)

        # Banner
        self.frm_banner = tk.Frame(self.frm_content, bg=config.COLOUR_BACKGROUND, height=180)
        self.frm_banner.pack(fill="x")
        self.cvs_bg = tk.Canvas(self.frm_banner, height=180)
        self.cvs_bg.pack(fill="both", expand=True)

        self.bg_image = ImageTk.PhotoImage(file="assets/home.png")
        self.cvs_bg.create_image(0, 0, image=self.bg_image, anchor="center")

        self.img_banner = ImageTk.PhotoImage(Image.open(f"assets/sprites/sprite_{mapping.sprite_type_map[self.pet.sprite].lower()}_{mapping.expr_map[self.pet.expression]}.png").resize((150, 150)))
        self.cvs_bg.create_image(500, 15, image=self.img_banner, anchor="ne")

        # Screen
        self.cvs_content = tk.Canvas(self.frm_content, bg=config.COLOUR_BACKGROUND)
        self.srcl_content = tk.Scrollbar(self, orient="vertical", command=self.cvs_content.yview)
        self.frm_screen = tk.Frame(self.cvs_content, bg=config.COLOUR_BACKGROUND)

        self.cvs_content.create_window((0, 0), window=self.frm_screen, anchor="nw")

        self.frm_screen.bind("<Configure>", lambda e: self.cvs_content.configure(scrollregion=self.cvs_content.bbox("all")))
        self.frm_screen.bind_all("<MouseWheel>", lambda e: self.cvs_content.yview_scroll(int(-1*(e.delta/100)), "units"))
        self.frm_screen_id = self.cvs_content.create_window((0, 0), window=self.frm_screen, anchor="nw")
        self.cvs_content.bind("<Configure>", lambda e: self.cvs_content.itemconfig(self.frm_screen_id, width=e.width))

        self.cvs_content.configure(yscrollcommand=self.srcl_content.set)

        self.cvs_content.pack(side="left", fill="both", expand=True)
        self.srcl_content.pack(side="right", fill="y")

        self.select_tab("Status")


    def select_tab(self, name: str):
        for tab_name in self.tabs:
            if tab_name == name:
                self.tabs[tab_name]["frame"].configure(bg=config.COLOUR_ACTIVE)
                self.tabs[tab_name]["lbl"].configure(bg=config.COLOUR_ACTIVE)
            else:
                self.tabs[tab_name]["frame"].configure(bg=config.COLOUR_INACTIVE)
                self.tabs[tab_name]["lbl"].configure(bg=config.COLOUR_INACTIVE)

        self.clear_tab()
        self.tabs[name]["command"]()

    def clear_tab(self):
        for widget in self.frm_screen.winfo_children():
            widget.destroy()

    def show_home(self):
        self.pet.save_config()
        self.root.selected_pet = None
        self.root.current_frame_num.set(config.FRAME_HOME)

    def show_settings(self):
        screens.pet_config.PetConfig(self.frm_screen, self, self.pet).pack(fill="x", expand=True)

    def show_interact(self):
        screens.pet_interact.PetInteract(self.frm_screen, self.pet).pack(fill="x", expand=True)

    def show_status(self):
        screens.pet_status.PetStatus(self.frm_screen, self.pet).pack(fill="x", expand=True)

    def show_journal(self):
        screens.pet_journal.PetJournal(self.frm_screen, self.pet).pack(fill="x", expand=True)

    def update_expression(self):
        self.img_banner = ImageTk.PhotoImage(Image.open(f"assets/sprites/sprite_{mapping.sprite_type_map[self.pet.sprite]}_{mapping.expr_map[self.pet.expression]}.png").resize((150, 150)))
        self.cvs_bg.create_image(500, 15, image=self.img_banner, anchor="ne")

        self.after(1000, self.update_expression)