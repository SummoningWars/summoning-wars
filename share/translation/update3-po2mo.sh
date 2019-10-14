#!/bin/bash

skipdirectories="potfiles xmlgettext extract"

if [ -z "$*" ]; then
	langs=$(ls -d */)
	langs=${langs//\//}
else
	for l in $*
	do
		if [ -d "$l" ]; then
			langs="$langs $l"
		else
			echo "$l is unknown!"
		fi
	done
fi

for l in $skipdirectories
do
	langs=${langs//$l/}
done

for l in $langs
do
	# get all po files from the directory and strip directory prefix
	pos=$(ls $l/*.po)
	pos=${pos//$l\//}
	[ -d "$l/LC_MESSAGES" ] || mkdir "$l/LC_MESSAGES"
	for p in $pos
	do
		echo -ne "\E[1;31m$l/$p \E[0m"
		msgfmt -c -v -o "$l/LC_MESSAGES/${p/%po/mo}" "$l/$p"
	done
	echo
done
echo
echo -e "\E[1;32mComplete.\E[0m"
echo
