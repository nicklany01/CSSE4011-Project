import tkinter as tk

from typing import Any

import config
import screens.homepage
import pet_setup
import screens.pet_page


class GUIWindow(tk.Tk):
    """The class reponsible for displaying the UI"""
    def __init__(self, pets: list[pet_setup.Pet]):
        """Initialises UI and its components and layout"""
        super().__init__()
        self.title("Lucina-Sienna")
        self.geometry(f"{config.WINDOW_WIDTH}x{config.WINDOW_HEIGHT}")
        # self.wm_attributes("-fullscreen", True)

        self.pets = pets
        self.selected_pet = None
        
        # Variable to track changes
        self.current_frame_num = tk.IntVar(value=config.FRAME_HOME)
        # Trace the variable to detect changes
        self.current_frame_num.trace_add("write", self.update_frame)
        self.current_frame: tk.Frame = screens.homepage.HomeScreen(self)
        self.current_frame.pack(expand=True, fill=tk.BOTH)

        self.focus_force()


    def clean_root(self) -> None:
        """Clears any components currently displayed in the UI"""
        for widget in self.winfo_children():
            widget.destroy()

    def update_frame(self, *args: tuple) -> None:
        """Callback function triggered when current_frame changes
        
        Parameters:
            *args(tuple): necessary callback information needed to correctly 
                call the function
        """

        self.current_frame.destroy()
        frame_index = self.current_frame_num.get()
        self.clean_root() 

        self.attributes("-topmost", False) # Aggressive otherwise
        self.focus_force() # Not aggressive enough

        screen = None

        if frame_index == config.FRAME_HOME:
            screen = screens.homepage.HomeScreen(self)
        elif frame_index == config.FRAME_PET_CONFIG:
            screen = screens.pet_page.PetPage(self)
        else:
            screen = screens.homepage.HomeScreen(self)
        
        self.current_frame = screen
        screen.pack(expand=True, fill=tk.BOTH)