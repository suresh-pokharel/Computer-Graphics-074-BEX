Calibration:
	. Open Calibrate/calibrate.pde
	. Use mouse to change camera view
	. Adjust kinect position such that your camera z-axis passes through the center of the object of interest.
	. Fix the kinect position

Genetate Cloud:
	. Open genetrate cloud/generate cloud.pde
	. The center of object from kinect needs to be set in the rotatey function and needs to be calibrated for removing alignment problems
	. Use keyboard to give inputs
	. Input keys:
		i : capture frame
		r : reset angle
	. Any nubmber entered at any time is taken as angle which is displayed in the terminal
	. Always reset angle at the start and before entering a new angle
	. Give only positive angles i.e. not -90deg but 270 deg
	. The angles are to be entered in degrees

Display:
	. Open display/display.pde
	. Set the loading location of the file to the previously created csv file

Requirements:
	Software
		. Processing IDE
		. Openkinect for processing (Library)
		. Peasy cam (Library)
	
	Hardware
		. Kinect V1
		. Mechanical Experimental setup
		