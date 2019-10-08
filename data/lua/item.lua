
--findet ein item bei einem Spieler und entfernt dieses.
--owner: der Spieler
--item: der gesuchte Subtyp
--return: true, wenn Item gefunden und geloescht, false wenn nicht
function findAndRemoveItem(owner,item)
	local itempos=searchPlayerItem(owner,item);
	if(itempos ~= 0)then
		getPlayerItem(owner,itempos);
		removePlayerItem(owner,itempos);
		deleteItem();
		return true;
	else
		return false;
	end;
end;

--checks whether a player owns an item
--owner: the player
--item: subtype of the item to be checked
--return: true, if player has item, false otherwise
function hasItem(owner,item)
	local itempos = searchPlayerItem(owner,item);
	if(itempos~=0)then
		return true;
	else
		return false;
	end;
end;

function hasCurrentItem()
	return (getItemValue("type") ~= "noitem");
end;
	