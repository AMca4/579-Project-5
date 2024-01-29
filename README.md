# EE579 ADVANCED MICROCONTROLLER PROJECT
## General project instructions and objectives
The following is essentially what we wrre tasked to do.

Build a vehicle capable of playing a game of skittles in the corridor outside RC6.41.
Two teams' vehicles will compete simultaneously, with a single set of 2 black metal “skittles” placed between 6 and 10 metres from both vehicles, and both vehicles beginning adjacent to each other. The skittles will consist of two tin soup cans soldered end to end and painted.

You will be given a model car to modify. You are expected to retro-fit this vehicle to operate autonomously, without altering the chassis (in particular, the wheel positions or bearings) or the drive train of the vehicle. Teams will be given two runs, alongside another vehicle. Runs will be 2 minutes long. During the heat, no team member will be permitted beyond the start line. Teams will score the number of complete seconds until they knock over the skittle, or two minutes per skittle if they do not knock over the skittle. The object is to get the lowest score in a round, and the lowest score from the two rounds will be counted. The top four teams will compete in a semi-final and the final two in the final.

In addition to two black skittles there will be a white skittle. Knocking over the white skittle will result in disqualification from the round.
+ Teams will see the position of the skittles one minute prior to the start of a round.
- The vehicle must operate autonomously. No external control may be given to the vehicle after it has been started.
- For the purposes of adjudicating a tie, the vehicle displacement from the start line at the end of the round will be measured, with longer distances scoring higher.

Marks for the project will be calculated on a group basis. The design and construction of the device will be marked out of 300. The team scoring the shortest time (or greatest displacement, should no team finish), will be given at additional 30 bonus marks. The group coming second will get 15 additional marks.

The design is unconstrained, except that your design may not run a general purpose operating system, such as Linux. Passive components can be requested from the workshop. Other components may be requested but a case may need to be made.

A Technical Documentation File must be produced for the design. This file must include:
- The specification of the design.
- Description of the hardware including circuit diagram and test points/instructions.
- Description of the software including a functional description.
- Usage instructions

The TDF is not a description of the construction of the project, but a description of the end product. A key assessment criteria of the TDF will be how easy it would be, using the TDF, to make a modification or extension to the design. It is expected that the TDF will be in the order of 25 to 30 pages, including diagrams.

In addition to the Technical Document File, a Policies and Procedures Document (PPD) must be produced documenting your group working practices, and in particular code versioning and review, and also measures taken during the design and construction phase to increase resilience to delays due to illness or other unforeseen circumstances. The PPD is expected to be in the order of one or two pages.

A descriptive report on the project is not required. However, the log books of each project member must be returned with the Technical Documentation File and it is expected that these log books will describe the design and debugging process.

By default, the group mark will be divided equally between all group members. However, group members may ask for a different proportional assignment of marks between group members at the time the project is submitted. It is up to group members themselves to make decisions regarding which group member should undertake which part of the design. The project is to be demonstrated during the exam diet. The Technical Documentation File must be submitted on or before the Monday of the week following the demo. The mark allocated to each student will be considered to be a percentage, 70% of which will go towards the final course mark. The remaining 30% of the final mark will come from the log book and assignments.

Room RC6.23 is allocated to the class on Friday for constructing and testing the hardware. Safety regulations require that more than one person be present in the room when any work is carried out - attending as a group satisfies this requirement.
## Setup instructions
We have used VS code for building this project. The following are the instructions for setting up the environment, if you wish to alter the code and flash it to the ESP32.
### Windows setup:
- Step 1: Download the latest version of [Python](https://www.python.org/downloads/) and [Git](https://git-scm.com/download/win) 
- Step 2: Download Espressif's Toolchain [here](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/windows-setup.html).
- Step 3: Make sure to open the installer with administrator rights. On the third screen, ***don't click on next before the scanning is complete!***. If there are no warnings you may procceed to the next step by clicking next. If you notice warnings, click on apply fixes and grant administrator rights on the pop-up screen. You may then proceed to the next step by clicking on next.
- Step 4: You may select to Download ESP-IDF or Use an existing ESP-IDF directory if you already have an installation on the ESP-IDF Tools.
- Step 5: ***This is a very crucial step!*** Make sure you make a note of the directory it wants to install to and select any of the above. Our recommendation is to select the latest release from (release branch - git clone).
- Step 6: Specify where you would like the ESP-IDF Tools to be installed and **make a note of it!**
- Step 7: You may select any of the options but we recommend you go for the full installation, always making sure to select the appropriate drivers for your own board.
- Step 8: In the *Ready to Install* screen verify all your choices by clicking Install. At this stage it's good to once more make a note of the location for the *ESP-IDF* and the *IDF_TOOLS_PATH*.
- Step 9: You'll now get a couple of pop-ups for granting administrator rights and another one for installing the drivers. Make sure to grant all of these. You can now let the installer download and install the ESP-IDF.
- Step 10: Download [VS code](https://code.visualstudio.com/download) and install using the default options.
- Step 11: Once the setup is complete, open VS code and click on Crtl+Shift+X navigate to the extentions tab. You can also find it on the left hand-side of VS Code. In the search bar type in ESP-IDF and install it.
- Step 12: You can now press Crtl+Shift+P (or navigate to the top of VS code and click on the search bar & click on *Show and Run Commands*) and type in *Configure ESP-IDF Extention*. Give it a second to load and then you'll be greeted with a welcome screen.
- Step 13: Choose *USE EXISTING SETUP* and then your installation should appear. Clicking on it, will install the necessary libraries for VS code to communicate with the ESP-IDF tools and once instructed you may close the window. You can skip to step 15.
- Step 14: If your installation doesn't appear, choose *Search ESP-IDF in system* and select the download server (*github if you followed our instructions above*) and *Find ESP-IDF in your system* from the dropdown. Now specify the locations for the *ESP-IDF* and the *IDF_TOOLS_PATH* from step 8 and click on install. Wait for it to finish and once instructed you may close the window.
- Step 15: Congratulations! You have successfully set-up *ESP-IDF* in your system!
