#!/bin/bash
echo
echo -e "\E[1;32mBuilding extract tool...\E[0m"
echo
make -C extract

echo
echo -e "\E[1;32mCreating new potfiles...\E[0m"
echo

# remove everything old...
rm potfiles/*.pot potfiles/*.poe

./extract/extract potfiles/ ../data
xgettext -d menu -o potfiles/menu.pot ../../src/core/*.cpp ../../src/core/*.h ../../src/gui/*.cpp ../../src/gui/*.h
xgettext -d tools -o potfiles/tools.pot ../../src/gui/contenteditor/*.cpp ../../src/gui/contenteditor/*.h ../../src/gui/debugpanel/*.cpp ../../src/gui/debugpanel/*.h ../../src/gui/debugpanel/filebrowser/*.cpp ../../src/gui/debugpanel/filebrowser/*.h

echo
echo -e "\E[1;32mMerging extracted files...\E[0m"
echo

epots=$(ls potfiles/*.poe)
for e in $epots
do
	pot=${e/%e/t}
	echo -ne "\E[1;31m$pot \E[0m\n"
	if [ -f "$pot" ]; then
		msgcat -F -o "$pot" "$pot" "$e"
	else
    cat "$e" > "$pot"
	fi
done

echo
echo -e "\E[1;32mMerging predefined include files...\E[0m"
echo

pois=$(ls potfiles/*.poi)
for e in $pois
do
	pot=${e/%i/t}
	echo -ne "\E[1;31m$e \E[0m"
	if [ -f "$pot" ]; then
		msgcat -F -o "$pot" "$pot" "$e"
	else
		msginit --no-translator -i "$e" -o "$pot"
	fi
done
echo
echo -e "\E[1;32mComplete.\E[0m"
echo
