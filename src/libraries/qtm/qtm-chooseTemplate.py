#! /usr/bin/env python

#   qtm-quickpost.py - Python script to launch a quickpost from the
#   Ubuntu Unity launcher.
#
#   Copyright (C) 2011, Matthew J Smith
# *
# * This file is part of QTM.
# * QTM is free software; you can redistribute it and/or modify
# * it under the terms of the GNU General Public License (version 2), as
# * published by the Free Software Foundation.
# *
# * This program is distributed in the hope that it will be useful,
# * but WITHOUT ANY WARRANTY; without even the implied warranty of
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# * GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA



import dbus
import os

qtm_started = False
sb = dbus.SessionBus()

while True:
  try:
    qtm = sb.get_object( 'uk.co.blogistan.catkin', '/MainApplication' )
  except dbus.exceptions.DBusException:
    if qtm_started == False:
      os.system( "qtm --no-new-window &" )
      qtm_started = True
    continue
  break
 
qtm.chooseQuickpostTemplate()

