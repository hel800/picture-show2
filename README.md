# picture-show2 - Windows Installer BRANCH 

This branch essentially contains a config file for Inno Setup Compiler (https://jrsoftware.org/isinfo.php) and the folder structure in order to create an installer package for picture-show2 for Windows. 

In the following, the process to create the installer is described in more detail.
General information for Qt deployment: https://doc.qt.io/qt-6/windows-deployment.html

## Gather all library and qml files from Qt
- create depoyment folder "deploy" next to the iss-config file and copy picture-show2.exe from release binary folder
- execute: windeployqt --qmldir <path-to-app-qml-files> <path-to-deploy-directory> in Qt env
  this will gather all requirements for the app

## Eliminate unecessary dll files
- execute: picture-show2.exe in the deployment folder to check that it is running (temporarily copy the exe file from the binary folder)
- search for all \*.dll files in the deployment folder
- try to delete these *.dll files; only unused dll files will be deleted
- now manually scan all subfolders in the deployment folder with they still contain *.dll files
- delete folders without \*.dll files

## Prepare installer
- copy msvc runtime dll files from system to the deployment folder
- copy exiv2.dll from the binary folder of the toolkit to the deployment folder
- adapt \*.iss project file to contain all remaining dependencies from the deployment folder
- don't forget to include the newest picture-show2.exe as well as the english translation file
- run the installer build