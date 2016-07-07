
mytetra_webengine == mytetra webengine == MyTetra WebEngine

A knowledge management system equipped with an embedded web browser derived from Qt::WebEngine (Chromium-based). A lightweight browser embedded in a note management application provides an easy way to combine tabs, bookmarks and history management, as well as internal and external search together.

Yes, it is a lightweight web browser.

The next step, I aim to append some real-time collaborative editing functions.

Should you need more functions, feel free to contact me: hughvonyoung@gmail.com


How to build mytetra_webengine
_______________________________________________________________________

1 Linux

Let's say the directory of your current project is named "working":

    working
        |____verdigris (created by or synchronized with git)
	|	|____src
	|
	|____knowledge management (name is whatever)
	        |____mytetra_webengine (created by or synchronized with git)


1.1 Install C++ Boost library (for boost::intrusive_ptr, boost::MPL, and some others. I did not build it)

1.2 Install verdigris (for remove qt::moc, just two header-only files)

    git clone https://github.com/woboq/verdigris.git

1.3 Download and install Qt 5.6

    http://download.qt.io/development_releases/qt/5.6/5.6.0-rc/qt-opensource-linux-x64-5.6.0-rc.run

    Install it to your Linux distribution

    Default location for administrator installation:

    /opt/Qt5.6.0

    or some user installation is up to you. But do not recommend to mix it up with system installation of Qt

    After installation, you'll get a "MaintenanceTool" under the installation root directory.

1.4 clone the repo to your workbench

    git clone https://github.com/beimprovised/mytetra_webengine.git

1.5 Compile the Qt project file with Qt-Creator which comes with Qt 5.6

    Open "mytetra.pro" under mytetra_webengine in qtcreator

    "Configure" the project in qtcreator (no additional requirements)

    Build it.

_______________________________________________________________________

mytetra_webengine is forked from MyTetra
_______________________________________________________________________

ENG: Central source repository for MyTetra development.
MyTetra official english page: [http://webhamster.ru/site/page/index/articles/projectcode/138](http://webhamster.ru/site/page/index/articles/projectcode/138)

RUS: Основной репозитарий исходников для разработки MyTetra.
Официальная русскоязычная страница программы MyTetra: [http://webhamster.ru/site/page/index/articles/projectcode/105](http://webhamster.ru/site/page/index/articles/projectcode/105)

Agreement on the coding (registration code)
_______________________________________________________________________

When writing code to be guided by the following agreements:

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

    ```
    #ifndef _CLIPBBRANCH_H_
    #define _CLIPBBRANCH_H_
    ...
    #endif // _CLIPBBRANCH_H_
    ```

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

Besides these two branches have been established in other branches for the implementation of a functional. Upon completion of the changes flow into the branch experimental. After stabilization of the changes in the experimental, the changes flow into the master and release is issued.

Deciding to connect to the development MyTetra, please use the experimental branch for the current code.
