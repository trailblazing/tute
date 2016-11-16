
**mytetra_webengine** == **mytetra webengine** == **MyTetra WebEngine**

A knowledge management system equipped with an embedded web browser derived from Qt::WebEngine (Chromium-based). A lightweight browser embedded in a note management application provides an easy way to combine tabs, bookmarks and history management, as well as internal and external search together.

Yes, it is a lightweight web browser.

The next step, I aim to append some real-time collaborative editing functions.

Should you need more functions, feel free to contact me: hughvonyoung@gmail.com

##How to remote synchronize mytetra_webengine
_______________________________________________________________________

Currently I choose Dropbox

Move fils to Dropbox:

	rsync -n -avhSDHAX  --progress --delete --ignore-errors --force --stats "./bin/" "~/dropbox/knowledge\ management/."

After that, Change settings from contextmenu: Main Menu: Tools: Main Preferences: Main: "Data directory" and "Trash directory". Point them to new "data" and "trash" folders.

##How to build mytetra_webengine
_______________________________________________________________________

###1 Build on Linux

Let's say the directory of your current project is named "working":

	working
	    |____verdigris (created by or synchronized with git) (does not needed for Qt 5.7)
	    |       |____src
	    |
	    |____knowledge management (name is whatever)
		    |____mytetra_webengine (created by or synchronized with git)

####1.1 Install C++ Boost library (for boost::intrusive_ptr, boost::MPL, and some others. I installed it from package manager)

####1.2 Install verdigris (for remove qt::moc in Qt 5.6. Two header-only files.)

	git clone https://github.com/woboq/verdigris.git

####1.3 Download and install Qt 5.7

	wget http://download.qt.io/official_releases/online_installers/qt-unified-linux-x64-online.run

* Run the installation. Default location for administrator installation (hereafter called "path-to-qt-installation"):

	/opt/Qt/5.7

but it's not a must

* Environment in ~/.profile (~/.bash_profile or ~/.bashrc)
	QT5DIR=/path-to-qt-installation/gcc_64

	export QT5DIR

	if ! echo ${PATH} | /bin/grep -q $QT5DIR/bin ; then
	    PATH=$QT5DIR/bin:${PATH}
	    PKG_CONFIG_PATH=$QT5DIR/lib/pkgconfig:${PKG_CONFIG_PATH}
	fi

	QT5INC="$QT5DIR/include"
	QT5LIB="$QT5DIR/lib"
	QT5LIB=$QT5DIR/plugins/platforms:${QT5LIB}
	PATH=$QT5INC:${PATH}
	PATH=$QT5LIB:${PATH}

	export QT5DIR QT5INC QT5LIB PATH
* Do not recommend to mix it up with system installation of Qt

####1.4 Clone the repo to your workbench
	git clone https://github.com/beimprovised/mytetra_webengine.git

####1.5 Compile

	/path-to-qt-installation/gcc_64/bin/qmake mytetra_webengine.pro

	make all

	sudo make install

* Or via GUI (Compile the Qt project file with Qt-Creator which comes with Qt 5.7):
        Open "mytetra_webengine.pro" under mytetra_webengine in qtcreator
        "Configure" the project in qtcreator (no additional requirements)
        Build it.

_______________________________________________________________________

##mytetra_webengine is forked from MyTetra
_______________________________________________________________________

ENG: Central source repository for MyTetra development.
MyTetra official english page: [http://webhamster.ru/site/page/index/articles/projectcode/138](http://webhamster.ru/site/page/index/articles/projectcode/138)

RUS: Основной репозитарий исходников для разработки MyTetra.
Официальная русскоязычная страница программы MyTetra: [http://webhamster.ru/site/page/index/articles/projectcode/105](http://webhamster.ru/site/page/index/articles/projectcode/105)

Agreement on the coding (registration code)

_______________________________________________________________________

###When writing code to be guided by the following agreements:

* Source file encoding: UTF-8;
* Comments in the code allowed the Russian and English languages;
* Tab prohibited padding spaces are made;
* Margin - 2 spaces;
* Comments must have the same indentation that of the code;
* The name of the file with the implementation of the class is based on the principle ClassName.cpp/classname.cxx;
* The name of the file with the title of class is based on the principle ClassName.hxx/classname.hxx;
* The name of the class is based on the principle ClassName;
* The name of the method is based on the principle method_name;
* The name of the variables is based on the principle _parameter_name;
* The opening "{" and closing bracket "}" should be on separate lines. It is unacceptable to place an opening bracket in the line for (), if (), etc .;
* Each newly created method should contain a comment that describes what the method does. So it is desirable to describe the purpose of the method parameters;
* The header class, after the section is necessary in #include class assignment to write comments.
* Each header file must have the following structure:

	#ifndef _CLIPBBRANCH_H_
	#define _CLIPBBRANCH_H_
	...
	#endif // _CLIPBBRANCH_H_

* Always use the classes, and classes instead of Qt's containers and containers STL;
* The program is designed as a cross-platform MyTetra Qt-only application. MyTetra program should not depend on third-party libraries that are not part of Qt. If you need to use a third-party library of its source code to the project.

Branches at GIT
_______________________________________________________________________

The system Git created two main branches.

For MyTetra

* MyTetra       - the main branch with the latest stable release;
* Experimental  - branch to develop.

For mytetra_webengine:

* mytetra_webengine - the main branch with the latest stable release;
* develop           - branch to develop.
* code beautifier   - https://github.com/beimprovised/mytetra_webengine/blob/mytetra_webengine/src/uncrustify.cfg

Besides these two branches have been established in other branches for the implementation of a functional. Upon completion of the changes flow into the branch experimental. After stabilization of the changes in the experimental, the changes flow into the master and release is issued.

Deciding to connect to the development MyTetra, please use the experimental/develop branch for the current code.
