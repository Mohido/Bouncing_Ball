# Bouncing_Ball
A simple cpenCV/c++ game in which you try to prevent the ball from falling to the abyss.

# Why creating the game?
The game is a simple assignment given in the course of 3D computer vission (first practice/class) at ELTE University. It is meant to practice the basics of using OpenCV with basic rendering in C++.

# How to run?
1) Make sure that you have OpenCV installed on your machine. I personally used OpenCV 4.5.3. I don't know the minimum OpenCV versino required for running this game; however, I suppose it may be version 3+ will be OK for running this application.
2) Simplest way to build the project and explore it is via Visual Studio 16+. You can download Visual studio coommunity and start the project in it.
3) Before building you need to configure the project "Include Directories", "Library Directories", and -last but not least- "Additional dependencies."
4) "Include Directories" and "Library Directories" should be in the project "properties" -> "Configuration Properties" -> "VC++ Directories". For the "include Directories": add the address to your installed "opencv/include" folder. And for the "Library Directories": add the address to your installed "opencv/x64/<VISUAL_STUDIO_VERSION>/lib". 
5) "Additional Dependencies" can be found in the project "properties" -> "Linker" -> "Input". Make sure to add the file name "opencv_world<OPENCV_VERSION>d.lib" in your "opencv/x64/<VISUAL_STUDIO_VERSION>/lib" folder. Make sure to read between the '<>' (This identifies a variable name) in between the "" signs. For instance, the opencv_world lib would like like this on my machine: "opencv_world453d.lib", since I am using version 4.5.3 as I mentioned above.
6) After editing the visual studio linker and dependencies, now we are ready to build the project. Make sure that the build is set on x64, then build the project. 
7) Enjoy the game and playing with the code. 


# Note:
The code has alot of imperfections in the game physics and will not be fixed. This project focuses on OpenCV not 2D game physics.

# Images: <br>
## Gameplay: <br>
![Gameplay](https://user-images.githubusercontent.com/48254077/132957811-033b12e2-d848-41e7-b345-ff34e5b0e7dd.png)
<br><hr><br><br>
## Gameover: <br>
![Gameending](https://user-images.githubusercontent.com/48254077/132957822-967a4305-483b-4eda-b1b8-1ef6f188f6a5.png)
<br><hr><br><br>
