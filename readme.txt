Original project OgreCrowd(https://github.com/duststorm/OgreCrowd) was update 7 years ago, and most the code is based on that.

This is the differences between this project and that older project:
1.This project is using OgreBites::ApplicationContext fits the master branch of ogre(https://github.com/OGRECave/ogre.git) instead of the older version (BaseApplication)
2.The main intention of this project is to validate to make recastnavigation(https://github.com/recastnavigation/recastnavigation.git) worked fine with ogre for python. So the pydemo/c directory is the c++ demo to validate which things should be done with python and the other should be done with python.
3.I keeped the original(OgreCrowd) code untouched as much as I can, but the class of AnimationCharactor is mix with Ogre too much. I need time to think which part of the jobs should be done in AnimationCharactor and the other should be left to users. Some original header still need to be modified to adjust for the python solution.
4.Python solution is similar with the demo2(https://github.com/niubaty/btogre), so the python demo of this project will be added soon in future or never :)
5.All of the original OgreCrowd code was moved to bakcup directory.

How to play with this demo:
0.Copy the backup/resources to your ogre sdk bin directory as the same as the  *.cfg files in that directory.
1.Default the debugFlag is true. So the demo will draw a lot of debug lines and colored blocks to help you understand.
2.With 't' pressed and left click to the dungen to mark start position for robot.
3.With 't' pressed and right click to the dungen to mark end position for robot to move.
4.Press 'c' , your will control the robot with UP/DOWN/LEFT/RIGHT
5.Press 'c' again, the robot will be navigated to the destination you marked before.

Notes:
This project is only tested in win10 with vs2015
Read the recastnavigation(https://github.com/recastnavigation/recastnavigation.git) will help a lot.
