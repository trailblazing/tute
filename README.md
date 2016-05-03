
mytetra_webengine == mytetra webengine == MyTetra WebEngine

A knowledge management system equipped with an embedded web browser derived from Qt::WebEngine (Chromium-based). A light weight browser embedded in a note management application provides an easy way to combine tabs, bookmarks and history management, as well as internal and external search together.

The next step, I aim to append some real-time collaborative editing functions.

Should you need more functions, feel free to contact me: hughvonyoung@gmail.com

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


How to compile the repo
_______________________________________________________________________

1 Linux

1.1 Install C++ Boost library version 1.60 (for boost::intrusive_ptr<SomeObjectType>, and some others maybe)

1.2 Download Qt 5.6

http://download.qt.io/development_releases/qt/5.6/5.6.0-rc/qt-opensource-linux-x64-5.6.0-rc.run

1.3 Install it to your Linux distribution

Default location for administrator installation:

/opt/Qt5.6.0

or some user installation is up to you. But do not recommend to mix it up with system installation of Qt

After installation, you'll get a "MaintenanceTool" under the installation root directory.

1.4 clone the repo to your workbench

git clone https://github.com/BeImprovised/mytetra_webengine.git

1.5 Compile the Qt project file with QtCreator which comes with Qt5.6

Open "mytetra.pro" in QtCreator

Configure the project

Build it.

