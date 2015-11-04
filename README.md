Papagayo with Spine Export
========

Lipsync tool for animators

Papagayo 2.0b1 with Spine Export
------------------------------------------------------------

This is a fork of LostMoho/Papagayo that adds an exporter for the Spine animation software(http://esotericsoftware.com/).

For more project and licencing info, please see the original repo: https://github.com/LostMoho/Papagayo


Using the Exporter
------------------------------------------------------------
To use the exporter, create or open a Papagayo project like you normally would, then select "Spine" form the export dropdown and press "Export".

The current voice name will be used as the name for the animation in the spine JSON file. 

A dialog will appear with export settings:

- Open JSON - This allows you to open an existing Spine JSON file that will populate the tree view with the bones from that skeleton. You can then select a bone from the treeview to attach the mouth animations to. 

- Image Dimensions - By default, Papagayo uses 200 x 200 images. If you are using custom images, you can specify their size here. 

- Export words as events - Spine does not support the playback of sound, so checking this will export all words as Spine events so it is easier to see what is being said. 

Click OK and choose the name of your export file. 

You can now import the JSON file into Spine.

To use custom mouth shapes simply name your mouth shapes as their phonemes(AI, E, etc, FV, L. MBP, O, rest, U, WQ) and add them to your Images folder in Spine. 

Building
------------------------------------------------------------
To build Papagayo:

Mac:

  1)Open up the file Papagayo.pro in Qt Creator and press the Build Project button.

Windows: 

  1) Open up the file Papagayo.pro in Qt Creator

  2) In the .pro file, update the location of the libsnd .dll with the path on your machine. 


  3) Press the Build Project button. 

  4) Copy /libsndfile_1.0.25/Win32/bin/libsnd-1.dll into the build output directory. 

  5) Open a terminal at the build output directory and run 'windeployqt Papagayo.exe'. (More info here: http://doc.qt.io/qt-5/windows-deployment.html#the-windows-deployment-tool)




