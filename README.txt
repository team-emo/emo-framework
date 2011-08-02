==============================================================
emo-framework: Lightweight 2D OpenGL game framework for Android / iOS
Copyright (c) 2011 emo-framework project
==============================================================

Home:        http://www.emo-framework.com/
Google Code: http://code.google.com/p/emo-framework/

emo-framework is licensed under the terms of the New BSD License (see LICENSE.txt).

= Install project template for Android =

Use Eclipse project template to create new emo project for Android.

Download the latest source distribution from http://code.google.com/p/emo-framework/downloads/list . The project template for Eclipse is in the Android-Template directory.

  * Start your Eclipse
  * Select 'File' -> 'Import' menu
  * Select 'General' -> 'Existing Projects into Workspace'

http://www.emo-framework.com/images/eclipse_install1.png

  * Choose 'Select root directory' and browse for the parent directory of the Android-Template directory.

http://www.emo-framework.com/images/eclipse_install2.png

  * Check the 'copy projects into workspace' if you want to copy them to your workspace. (Normally you may need to check it.)
  * Check the 'Android-Template' directory and press 'Finish' button

http://www.emo-framework.com/images/eclipse_install3.png

Edit package name in the !AndroidManifest.xml. Otherwise "com.emo_framework.examples" is used for your app. (you won't be happy  with it...)
{{{
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
        package="com.emo_framework.examples"
        android:versionCode="1"
        android:versionName="1.0">
}}}

The main script file is *assets/main.nut*. Edit it to create your own game.

http://www.emo-framework.com/images/eclipse_install4.png

{{{
function emo::onLoad() {
  // print log into the logcat
  print("It works!");
  emo.Stage().load(Main());
}
}}}


= Build examples for Android =

The example for Eclipse is in the Android-Examples directory.

  * Start your Eclipse
  * Select 'File' -> 'Import' menu
  * Select 'General' -> 'Existing Projects into Workspace'

http://www.emo-framework.com/images/eclipse_install1.png

  * Choose 'Select root directory' and browse for the parent directory of the Android-Examples directory.

http://www.emo-framework.com/images/eclipse_install2.png

  * Check the 'copy projects into workspace' if you want to copy them to your workspace. (Normally you may need to check it.)
  * Check the 'Android-Examples' directory and press 'Finish' button

= Install project template for Xcode 3 =

Use project template for Xcode 3 to create emo project for iOS.

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

After installing project template into your Xcode 3, you can choose the User Template to create new emo project for iOS.

http://www.emo-framework.com/images/xcode_install1.png

http://www.emo-framework.com/images/xcode_install2.png

The main script file is *Resources/main.nut*. Edit main.nut to create your own game. 

http://www.emo-framework.com/images/xcode_install3.png

{{{
function emo::onLoad() {
  // print log into the console
  print("It works!");
  emo.Stage().load(Main());
}
}}}

Have fun with emo!

http://www.emo-framework.com/images/xcode_install4.png

= Using with Xcode 4 =

emo-framework 0.1.2 and above contains project template for Xcode 4.

Download the latest source distribution from http://code.google.com/p/emo-framework/downloads/list . You can install the project template for Xcode 4 by using project template installer.

  * Open Terminal
  * In the terminal cd to the directory where you extract the source distribution. It goes something like below:

{{{
$ cd ~/Downloads/emo-framework-X.X.X-source
}}}

  * type ./install-templates-Xcode4.sh

{{{
$ ./install-templates-Xcode4.sh
}}}

If you want to re-install the project template, add -f option.

{{{
$ ./install-templates-Xcode4.sh -f
}}}

After installing project template into your Xcode 4, you can choose the User Template to create new emo project for iOS.

The main script file is *Resources/main.nut*. Edit main.nut to create your own game. 

http://www.emo-framework.com/images/xcode4_install2.png

{{{
function emo::onLoad() {
  // print log into the console
  print("It works!");
  emo.Stage().load(Main());
}
}}}

Have fun with emo!
