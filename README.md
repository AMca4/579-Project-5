# EE579 ADVANCED MICROCONTROLLER PROJECT
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
14. Step 14: If your installation doesn't appear, choose *Search ESP-IDF in system* and select the download server (*github if you followed our instructions above*) and *Find ESP-IDF in your system* from the dropdown. Now specify the locations for the *ESP-IDF* and the *IDF_TOOLS_PATH* from step 8 and click on install. Wait for it to finish and once instructed you may close the window.
15. Step 15: Congratulations! You have successfully set-up *ESP-IDF* in your system!
