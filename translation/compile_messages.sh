#!/bin/bash

# compiles *.po to *.mo binary without updating message-strings

# New *.po files (resulting from new domains) need to be added by hand.

while read LINE; do
	LANGUAGE="de"
	echo "+++ "$LINE" - GERMAN +++"
	echo "[ MO ]"
	msgfmt -c -v -o $LANGUAGE/LC_MESSAGES/$LINE.mo $LANGUAGE/$LINE.po

	LANGUAGE="it"
	echo "+++ "$LINE" - ITALIAN +++"
	echo "[ MO ]"
	msgfmt -c -v -o $LANGUAGE/LC_MESSAGES/$LINE.mo $LANGUAGE/$LINE.po

	LANGUAGE="ru"
	echo "+++ "$LINE" - RUSSIAN +++"
	echo "[ MO ]"
	msgfmt -c -v -o $LANGUAGE/LC_MESSAGES/$LINE.mo $LANGUAGE/$LINE.po


	LANGUAGE="pt"
	echo "+++ "$LINE" - PORTUGUESE +++"
	echo "[ MO ]"
	msgfmt -c -v -o $LANGUAGE/LC_MESSAGES/$LINE.mo $LANGUAGE/$LINE.po

	LANGUAGE="uk"
	echo "+++ "$LINE" - UKRAINIAN +++"
	echo "[ MO ]"
	msgfmt -c -v -o $LANGUAGE/LC_MESSAGES/$LINE.mo $LANGUAGE/$LINE.po
done < domains


echo "... Finished"

