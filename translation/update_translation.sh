#!/bin/bash

# New *.po files (resulting from new domains) need to be added by hand.


echo "Update translations ..."

echo Be shure you have installed dos2unix package

read -p "Press enter to continue" nothing

while read LINE; do
	LANGUAGE="de"
	echo "+++ "$LINE" - GERMAN +++"
	echo "[ PO ]"
	msgmerge -v -U $LANGUAGE/$LINE.po en/$LINE.pot
	unix2dos $LANGUAGE/$LINE.po
	echo "[ MO ]"
	msgfmt -c -v -o $LANGUAGE/LC_MESSAGES/$LINE.mo $LANGUAGE/$LINE.po

	LANGUAGE="it"
	echo "+++ "$LINE" - ITALIAN +++"
	echo "[ PO ]"
	msgmerge -v -U $LANGUAGE/$LINE.po en/$LINE.pot
	unix2dos $LANGUAGE/$LINE.po
	echo "[ MO ]"
	msgfmt -c -v -o $LANGUAGE/LC_MESSAGES/$LINE.mo $LANGUAGE/$LINE.po

	LANGUAGE="ru"
	echo "+++ "$LINE" - RUSSIAN +++"
	echo "[ PO ]"
	msgmerge -v -U $LANGUAGE/$LINE.po en/$LINE.pot
	unix2dos $LANGUAGE/$LINE.po
	echo "[ MO ]"
	msgfmt -c -v -o $LANGUAGE/LC_MESSAGES/$LINE.mo $LANGUAGE/$LINE.po

	LANGUAGE="pt"
	echo "+++ "$LINE" - PORTUGUESE +++"
	echo "[ PO ]"
	msgmerge -v -U $LANGUAGE/$LINE.po en/$LINE.pot
	unix2dos $LANGUAGE/$LINE.po
	echo "[ MO ]"
	msgfmt -c -v -o $LANGUAGE/LC_MESSAGES/$LINE.mo $LANGUAGE/$LINE.po

	LANGUAGE="uk"
	echo "+++ "$LINE" - UKRAINIAN +++"
	echo "[ PO ]"
	msgmerge -v -U $LANGUAGE/$LINE.po en/$LINE.pot
	unix2dos $LANGUAGE/$LINE.po
	echo "[ MO ]"
	msgfmt -c -v -o $LANGUAGE/LC_MESSAGES/$LINE.mo $LANGUAGE/$LINE.po
done < domains


echo "... Finished"

