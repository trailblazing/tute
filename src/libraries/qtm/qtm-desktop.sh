#!/bin/sh
cat <<EOF
[Desktop Entry]
Version=1.0
Type=Application
Name=QTM
GenericName=Blog editor
Comment=Weblog management application
TryExec=$1/bin/qtm
Exec=$1/bin/qtm
Categories=Application;Network;X-MandrivaLinux-Internet-Other;
Icon=$1/share/icons/qtm-logo1.png
# MimeType=image/x-foo;
# X-KDE-Library=libfooview
# X-KDE-FactoryName=fooviewfactory
# X-KDE-ServiceType=FooService
EOF

if [ "$2" = "--unity" ]; then
  cat <<EOF

X-Ayatana-Desktop-Shortcuts=NewQTMDocument;OpenQTMDocument;OpenRecentQTMDocument;QTMQuickpost;QTMQuickpostFromTemplate;ReallyQuitQTM

[NewQTMDocument Shortcut Group]
Name=New Document
Exec=qtm-new
TargetEnvironment=Unity

[OpenQTMDocument Shortcut Group]
Name=Open
Exec=qtm-choose
TargetEnvironment=Unity

[OpenRecentQTMDocument Shortcut Group]
Name=Open recent file...
Exec=qtm-chooseRecent
TargetEnvironment=Unity

[QTMQuickpost Shortcut Group]
Name=Quickpost
Exec=qtm-quickpost
TargetEnvironment=Unity

[QTMQuickpostFromTemplate Shortcut Group]
Name=Quickpost from template
Exec=qtm-chooseTemplate
TargetEnvironment=Unity

[ReallyQuitQTM Shortcut Group]
Name=Really quit QTM
Exec=qdbus uk.co.blogistan.catkin /MainApplication quit
TargetEnvironment=Unity

EOF
fi

