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
Categories=Network;News;
Icon=$XDG_DATA_DIRS/icons/qtm-logo1.png
X-SuSE-translate=false
# MimeType=image/x-foo;
# X-KDE-Library=libfooview
# X-KDE-FactoryName=fooviewfactory
# X-KDE-ServiceType=FooService
EOF
