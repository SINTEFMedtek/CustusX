# fedora-livecd-kde.ks
#
# Description:
# - Fedora Livecd Spin with the K Desktop Environment (KDE)
#
# Maintainer(s):
# - Sebastian Vahl <fedora@deadbabylon.de>
# - Fedora KDE SIG, http://fedoraproject.org/wiki/SIGs/KDE, kde@lists.fedoraproject.org

%include fedora-live-kde-base.ks
%include fedora-live-minimization.ks


%packages
# unwanted packages from @kde-desktop
# don't include these for now to fit on a cd
-desktop-backgrounds-basic
-kdeaccessibility*
-kdeedu*			# kdeedu is too big for the live images
-scribus			# scribus is too big for the live images
-kdeartwork-screensavers	# screensavers are not needed on live images
#-ktorrent			# kget has also basic torrent features (~3 megs)
#-digikam			# digikam has duplicate functionality with gwenview (~28 megs)
#-amarok 			# amarok has duplicate functionality with juk (~23 megs)
#-kipi-plugins			# ~8 megs
#-konq-plugins			# ~2 megs
#-kdeplasma-addons		# ~16 megs

# Additional packages that are not default in kde-desktop but useful
k3b				# ~15 megs
-koffice-suite			# don't include whole koffice-suite, just parts of it
koffice-kword
koffice-kspread			# ~1 megs
koffice-kpresenter		# ~3 megs
koffice-filters
#kdeartwork			# only include some parts of kdeartwork
#twinkle			# (~10 megs)
fuse
liveusb-creator
#pavucontrol			# pavucontrol has duplicate functionality with kmix
#kaffeine*			# kaffeine has duplicate functionality with dragonplayer (~3 megs)

# only include kdegames-minimal
-kdegames
kdegames-minimal

# use yum-presto by default
yum-presto

### space issues

# fonts (we make no bones about admitting we're english-only)
wqy-microhei-fonts	# a compact CJK font, to replace:
-un-core-dotum-fonts	# Korean
-vlgothic-fonts		# Japanese
-wqy-zenhei-fonts	# Chinese

-paratype-pt-sans-fonts	# Cyrillic (already supported by DejaVu), huge
#-stix-fonts		# mathematical symbols

# remove input methods to free space
-@input-methods
-scim*
-m17n*
-ibus*
-iok

# save some space (from @base)
-make
-nss_db

## avoid serious bugs by omitting broken stuff

%end

%post
%end
