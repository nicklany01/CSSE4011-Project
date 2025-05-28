import tkinter as tk

import pet_setup
import config
import club.pet_app_helpers

class PetJournal(tk.Frame):
    def __init__(self, root: tk.Frame, pet: pet_setup.Pet):
        super().__init__(root, bg=config.COLOUR_BACKGROUND)

        self.root = root
        self.pet = pet

        # tk.Label(self, text=f"{pet.name}'s Journal Entries", bg=config.COLOUR_BACKGROUND, font=("Consolas", 16)).pack(anchor="w", padx=20, pady=(20, 0))
        frm_header = tk.Frame(self, bg=config.COLOUR_ACTIVE)
        frm_header.pack(padx=20, pady=20, fill="x", expand=True)
        lbl_header = tk.Label(frm_header, text=f"{pet.name}'s Journal Entries", font=("Consolas", 16, "bold"), bg=config.COLOUR_ACTIVE)
        lbl_header.pack(ipady=20)

        self.frm_journal = tk.Frame(self, bg=config.COLOUR_INACTIVE, padx=20, pady=20)
        self.frm_journal.pack(fill="x", expand=True, pady=(0, 20), padx=20)

        self.write_journal()
        self.after(2000, self.update_display)


    def add_entry(self, journal_entry: club.pet_app_helpers.PetJournalEntry):
        frm_entry = tk.Frame(self.frm_journal, bg=config.COLOUR_INACTIVE)
        frm_entry.pack(fill="x", pady=5)

        tk.Label(frm_entry, text=f"({journal_entry.timestamp})", bg=config.COLOUR_INACTIVE, font=("Consolas", 12), width=7, anchor="w").pack(side="left")
        tk.Label(frm_entry, text=f"{journal_entry}", bg=config.COLOUR_INACTIVE, font=("Consolas", 12)).pack(side="left")

    def write_journal(self):
        count = 0
        for journal_entry in self.pet.journal.entries:
            if journal_entry is None:
                continue
            count += 1
            self.add_entry(journal_entry)

        if count == 0:
            tk.Label(self.frm_journal, text=f"Nothing to write today...", bg=config.COLOUR_INACTIVE, font=("Consolas", 12)).pack(pady=5, side="left")


    def update_display(self):
        for widget in self.frm_journal.winfo_children():
            widget.destroy()
        
        self.write_journal()
        
        self.after(2000, self.update_display)
