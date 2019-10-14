function printlist(tab)
	for key,value in pairs(tab) do
		print(key,value);
	end;
end;

function jump(region, location)
	player = getPlayers()[1];
	teleportPlayer(player,region,location);
end;

function camp()
	player = getPlayers()[1];
	teleportPlayer(player,"medMercCamp","entry_north");
end;

function gold(amount)
	plpos()
	set(pl,"gold",amount)
end;

function cam(location, distance)
	if(distance == nil)then
		distance = 18;
	end;
	addCameraPosition(0,position,-90,60,distance)
end;

function plpos()
	pl = getPlayers()[1];
	pos = getPosition(pl);
end;