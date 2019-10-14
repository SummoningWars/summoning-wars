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

pots=$(ls potfiles/*.pot)
pots=${pots//potfiles\//}

for l in $langs
do
	for p in $pots
	do
		po=$l/${p%t}
		# create the file if it doesn't exist
		echo -ne "\E[1;31m$po \E[0m"
		if [ -f "$po" ] ; then
			msgmerge --force-po --lang="$l" -F -U "$po" "potfiles/$p"
		else
			msginit -l "$l" -i "potfiles/$p" -o "$po"
		fi
		#unix2dos $po
	done
	echo
done
echo
echo -e "\E[1;32mComplete.\E[0m"
echo
