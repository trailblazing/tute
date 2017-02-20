
**Tute** <= **mytetra_webengine** <= **MyTetra**

It's a note.

It is a lightweight web browser. A knowledge management system equipped with an embedded web browser derived from Qt::WebEngine (Chromium-based). A lightweight browser embedded in a note management application provides an easy way to combine tabs, bookmarks and history management, as well as internal and external search together.

It's a client. You can manage your remote service with XML-RPC.

The next step, I aim to append some real-time collaborative editing functions.

Should you need more functions? feel free to contact me: hughvonyoung[at]gmail.com

Please note that Tute is distributed under the GPL v3.

##How to remote synchronize Tute
_______________________________________________________________________

Currently I choose Dropbox

Move files to Dropbox:
```
	rsync -n -avhSDHAX  --progress --delete --ignore-errors --force --stats "./bin/" "~/dropbox/knowledge\ management/."
```
After that, change settings from contextmenu: Main Menu: Tools: Main Preferences: Main: "Data directory" and "Trash directory". Point them to new "data" and "trash" folders.

##How to build Tute
_______________________________________________________________________

###1 Build on Linux

Let's say the directory of your current project is named "working":
```
	working
	    |____verdigris (created by or synchronized with git) (does not needed for Qt 5.7)
	    |       |____src
	    |
	    |____knowledge management (name is whatever)
		    |____tute (created by or synchronized with git)
```
####1.1 Install C++ Boost library (for boost::intrusive_ptr, boost::MPL, and some others. I installed it from package manager)

####1.2 Install verdigris (for remove qt::moc in Qt 5.6. Two header-only files.)
```
	git clone https://github.com/woboq/verdigris.git
```
####1.3 Download and install Qt 5.7.0
```
        wget http://download.qt.io/official_releases/qt/5.7/5.7.0/qt-opensource-linux-x64-5.7.0.run
```
* Run the installation. Default location for administrator installation (hereafter called "path-to-qt-installation"):
```
	/opt/Qt/5.7
```
but it's not a must

* Environment in ~/.profile (~/.bash_profile or ~/.bashrc)
```
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
```
* Do not recommend to mix it up with system installation of Qt

####1.4 Clone the repo to your workbench
```
	git clone https://github.com/beimprovised/tute.git
```
####1.5 Compile
```
	/path-to-qt-installation/gcc_64/bin/qmake tute.pro

        make -j$(nproc) all

	sudo make install
```
* Or via GUI (Compile the Qt project file with Qt-Creator which comes with Qt 5.7 or more recent version):
        Open "tute.pro" under tute in qtcreator
        "Configure" the project in qtcreator (append "-j*" with prefer process number * to "make" options might be better)
        Build it.

_______________________________________________________________________

##Tute is forked from mytetra_webengine and mytetra_webengine is forked from MyTetra
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
* The name of the file with the implementation of the class is based on the principle ClassName.cpp/classname.cxx (prefer: class_name.cxx);
* The name of the file with the title of class is based on the principle ClassName.hxx/classname.hxx/classname.h (prefer: class_name.hxx);
* The name of the class is based on the principle ClassName;
* The name of the method is based on the principle method_name (underscore);
* The name of the variables is based on the principle _parameter_name (underscore);
* The opening "{" and closing bracket "}" should be on separate lines. It is unacceptable to place an opening bracket in the line for (), if (), etc .;
* Each newly created method should contain a comment that describes what the method does. So it is desirable to describe the purpose of the method parameters;
* The header class, after the section is necessary in #include class assignment to write comments.
* Each header file must have the following structure:
```
	\#ifndef _CLIPBBRANCH_H_

	\#define _CLIPBBRANCH_H_

	...

	\#endif // _CLIPBBRANCH_H_
```
* Always use the classes, and classes instead of Qt's containers and containers STL;
* The program is designed as a cross-platform Qt-only application. Program should not depend on third-party libraries that are not part of Qt. If you need to use a third-party library of its source code to the project.

Branches at GIT
_______________________________________________________________________

The system Git created two main branches.

For mytetra_webengine

* origin       - the main branch with the latest stable release;
* develop      - branch to develop.

For Tute:

* master            - the main branch with the latest stable release;
* develop           - branch to develop.
* code beautifier   - uncrustify (recommended)
```
        find . -iname "*.h" -or -iname "*.cpp" -or -iname "*.cxx" > files.txt
        uncrustify -c src/uncrustify.cfg  -l CPP --replace --no-backup -F ./files.txt
```
* code beautifier   - clang-format::WebKit (to save time)
```
        find . -iname "*.h" -or -iname "*.cpp" -or -iname "*.cxx" | xargs clang-format -i -style=WebKit
```

Besides these two branches have been established in other branches for the implementation of a functional. Upon completion of the changes flow into the branch experimental. After stabilization of the changes in the experimental, the changes flow into the master and release is issued.

Deciding to connect to the development Tute, please use the develop branch for the current code.
