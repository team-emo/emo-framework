==============================================================
emo-framework: Lightweight 2D OpenGL game framework for Android / iOS
Copyright (c) 2011 emo-framework project
==============================================================

Home:        http://www.emo-framework.com/
Google Code: http://code.google.com/p/emo-framework/

emo-framework is licensed under the terms of the New BSD License (see LICENSE.txt).

= Install project template for Android =

Project template for Eclipse can be used to create new emo project for Android.

Download the latest source distribution from http://code.google.com/p/emo-framework/downloads/list . The project template for Eclipse is in the Android-Template directory.

  * Start your Eclipse
  * Select 'File' -> 'Import' menu
  * Select 'General' -> 'Existing Projects into Workspace'
  * Choose 'Select root directory' and browse for the parent directory of the Android-Template directory.
  * Check the 'copy projects into workspace' if you want to copy them to your workspace. (Normally you may need to check it.)
  * Check the 'Android-Template' directory and press 'Finish' button

Edit package name in the !AndroidManifest.xml. Otherwise "com.emo_framework.examples" is used for your app. (you won't be happy  with it...)
{{{
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
        package="com.emo_framework.examples"
        android:versionCode="1"
        android:versionName="1.0">
}}}

The main script file is assets/main.nut. Edit it to write your own game.

{{{
function emo::onLoad() {
  // print log into the logcat
  print("It works!");
}
}}}

That's all!

= Build examples for Android =

The example for Eclipse is in the Android-Examples directory.

  * Start your Eclipse
  * Select 'File' -> 'Import' menu
  * Select 'General' -> 'Existing Projects into Workspace'
  * Choose 'Select root directory' and browse for the parent directory of the Android-Examples directory.
  * Check the 'copy projects into workspace' if you want to copy them to your workspace. (Normally you may need to check it.)
  * Check the 'Android-Examples' directory and press 'Finish' button

= Install project template for Xcode 3 =

Project template for Xcode 3 can be used to create emo project for iOS.

Download the latest source distribution from http://code.google.com/p/emo-framework/downloads/list . You can install the project template for Xcode 3 by using project template installer.

  * Open Terminal
  * In the terminal cd to the directory where you extract the source distribution. It goes something like below:

{{{
$ cd ~/Downloads/emo-framework-X.X.X-source
}}}

  * type ./install-templates-Xcode3.sh -u

{{{
$ ./install-templates-Xcode3.sh -u
}}}

The -u option installs the project template into the user's Library directory: ~/Library/Application Support/Developer/Shared/Xcode .

If you want to install into global directory, type following. This installs the template into /Library/Application Support/Developer/Shared/Xcode .

{{{
$ sudo ./install-templates-Xcode3.sh
}}}

If you want to re-install the project template, add -f option.

{{{
$ ./install-templates-Xcode3.sh -u -f
}}}

This installs the project template into your Xcode 3. Now you can choose the project template to create new emo project for iOS.
