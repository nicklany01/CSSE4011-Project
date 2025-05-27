import window
import pet_setup
     


def comms_setup():
	pass


if __name__ == '__main__':
	pet0 = pet_setup.Pet("data/pet_0.json")
	pet1 = pet_setup.Pet("data/pet_1.json")
	pet2 = pet_setup.Pet("data/pet_2.json")
	
	gui_screen = window.GUIWindow([pet0, pet1, pet2])
	
	# Async BLE comms
	comms_setup()

    # Start the Tkinter main loop
	gui_screen.mainloop()
	
