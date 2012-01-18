###########################################################
# CustusX 15 Fedora Gnome3 Remix 
# Maintained by Christian Askeland <christian.askeland@sintef.no>
#
# Based on the Omega remix
# This remix is not affliated or endorsed by Red Hat
# Create your own remix - http://fedoraproject.org/wiki/How_to_create_and_use_a_Live_
###########################################################

#services --enabled=NetworkManager --disabled=rpcsvcgssd,rpcgssd,rpcidmapd,sshd,network,nfslock,nfs,rpcbind,sendmail

#######################################
# repositories

#######################################
# include other kickstarts

%include fedora-live-desktop.ks
%include fedora-live-minimization.ks
%include CustusX-base.ks



#######################################
# Settings defined using system-config-kickstart

# these settings seem to be applied only to the live image. What about the hdd install???
# Solution: http://forums.fedoraforum.org/showthread.php?t=221155
# Root password (standard CustusX root pw)
#rootpw --iscrypted $1$aaQdA8dB$2EcZecZF18ipqbR3IEMia/
# System authorization information
#auth  --useshadow  --passalgo=md5
# Use graphical install
#graphical
#firstboot --disable
# System keyboard
#keyboard no
# System language
#lang en_US
# Reboot after installation
#reboot
# System timezone
#timezone  Europe/Oslo


#######################################
%packages
#######################################

#######################################
# Utilities

@gnome-desktop

#######################################
# Removed from the standard installation

#-evolution - leads to a black spot in the shortcut in the panel
-gnome-games
-kdegames


#######################################
# Stuff from Omega


# Temporary list of things removed from comps but not synced yet
-specspo
# Add the Java plugin
java-1.6.0-openjdk-plugin
java-1.6.0-openjdk



# No printing
-foomatic-db-ppds
-foomatic

# Dictionaries are big
-aspell-*
-hunspell-*
### -man-pages*
-words

# Help and art can be big, too
-gnome-user-docs
-evolution-help
-gnome-games-help
-desktop-backgrounds-basic


# Legacy cmdline things we don't want
-nss_db
-krb5-auth-dialog
-krb5-workstation
-pam_krb5
-quota
-minicom
-dos2unix
-finger
-ftp
-jwhois
-mtr
-pinfo
-rsh
-telnet
-nfs-utils
-ypbind
-yp-tools
-rpcbind
-acpid

#######################################
%end
#######################################


