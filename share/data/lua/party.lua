-- Gibt einen Sprecher fuer eine Rolle zurueck, dessen ID nicht in der Liste forbiddenids ist
-- role: string, Sprecherrolle
-- availableids: Tabelle, int->bool, falls fuer eine ID true angegeben ist, so darf dieser Spieler gewaehlt werden
-- forbiddenids: Tabelle, int->bool, falls fuer eine ID true angegeben ist, so wird dieser Spieler nicht gewaehlt
function getRolePlayer(role, availableids,forbiddenids)
	local cand = getRolePlayers(role);
	local id,key
	for key,id in pairs(cand) do
		if ((availableids == nil or availableids[id] == true) and 
			not (type(forbiddenids) == "table" and forbiddenids[id] ~= true)
				  and not (type(forbiddenids) == "number" and  id ~= forbiddenids))then
			return id;
		end;
	end;
	return 0;
end;

-- Waehlt einen Spieler, wobei die Spieler in nonprefids nicht bevorzugs werden
function getRolePlayerNonPref(role,availableids, nonprefids)
	local id = getRolePlayer(role,availableids,nonprefids);
	if (id == 0) then
		id = getRolePlayer(role,availableids);
	end;
	return id;
end;

-- Gibt einen Spieler mit der passenden Rolle aus, wobei der Spieler mit der angegebenen ID bevorzugt wird
function getRolePlayerPref(role,availableids, prefid)
	local cand = getRolePlayers(role,availableids);
	local id,key
	for key,id in pairs(cand) do
		if (id == prefid) then
			return id;
		end;
	end;
	if (cand[1] == nil) then
		return 0;
	end;
	return cand[1];
end;

-- Teleportiert die komplette Party an die Position location in Region region
function assembleParty(region,location)
	if (region ~= nil and location ~= nil) then
		local players = getPlayers();
		local i,player;
		local tele = false;
		local tmp
		for i,player in ipairs(players) do
			tmp = teleportPlayer(player,region,location);
			if (tmp == true) then
				tele = true
			end;
		end;
		if (not tele) then
			insertTrigger("party_complete");
		end;
	end;
end;

-- Gruppiert Objekte in einem Teilkreis
-- objects - Liste der Objekte
-- location - Mittelpunkt des Bogens
-- lookatloc - Mittelpunks des Kreises
-- beware: if called directly in a party_complete-event, this may not work as intended
function groupObjectsArc(objects,location,lookatloc,radius, arcangle)
	if (location ~= nil) then
		location = locnameToLocation(location)
		lookatloc = locnameToLocation(lookatloc);
		local dir = {location[1] -lookatloc[1], location[2]-lookatloc[2]};
		local angle = math.atan2(dir[2],dir[1])*180/math.pi;
		local i,obj;
		local loc ={};
		if (radius == nil) then
			radius = math.sqrt(dir[1]*dir[1] + dir[2]*dir[2]);
		end;
		generateArc(loc,lookatloc,radius,table.getn(objects),arcangle,angle);
		
		-- zweimal ausfuehren, weil beim ersten Mal haeuffig noch Verdraengung stattfindet
		for i,obj in ipairs(objects) do
			moveObject(obj,loc[i]);
		end;
		for i,obj in ipairs(objects) do
			moveObject(obj,loc[i]);
			lookAt(obj,lookatloc);
		end;
		
		
	end;
end;

-- Fuegt alle Spieler dem Dialog hinzu
function addAllPlayersToDialog()
	local players = getPlayers();
	local i,player;
	for i,player in ipairs(players) do
		addSpeaker(player,get(player,"name"),true);
	end;
end;

--players: indizierte Liste der Spieler
function addPlayersToDialog(players)
	local i,player;
	for i,player in ipairs(players) do
		addSpeaker(player,get(player,"name"),true);
	end;
end;

function addStandardRoles(availableids)
	local leader = getRolePlayer("leader",availableids);
	local blockedspeaker={};
	blockedspeaker[leader] = true;
	addSpeaker(leader,"PL");
	addSpeaker(leader,"leader");
	
	local warrior = getRolePlayerNonPref("warrior",availableids,blockedspeaker);
	local mage = getRolePlayerNonPref("mage",availableids,blockedspeaker);
	local archer = getRolePlayerNonPref("archer",availableids,blockedspeaker);
	local priest = getRolePlayerNonPref("priest",availableids,blockedspeaker);
	local male = getRolePlayerNonPref("male",availableids,blockedspeaker);
	local female = getRolePlayerNonPref("female",availableids,blockedspeaker);
	local any = getRolePlayerNonPref("all",availableids,blockedspeaker);
	
	addSpeaker(warrior,"warrior",true);
	addSpeaker(mage,"mage",true);
	addSpeaker(archer,"archer",true);
	addSpeaker(priest,"priest",true);
	addSpeaker(any,"any",true);
	addSpeaker(male,"male",true);
	addSpeaker(female,"female",true);
	
	local magopt = getRolePlayerPref("all",availableids,mage);
	local waropt = getRolePlayerPref("all",availableids,warrior);
	local arcopt = getRolePlayerPref("all",availableids,archer);
	local priopt = getRolePlayerPref("all",availableids,priest);
	local malopt = getRolePlayerPref("all",availableids,male);
	local femopt = getRolePlayerPref("all",availableids,female);

	addSpeaker(magopt,"magopt",true);
	addSpeaker(waropt,"waropt",true);
	addSpeaker(arcopt,"arcopt",true);
	addSpeaker(priopt,"priopt",true);
	addSpeaker(malopt,"malopt",true);
	addSpeaker(femopt,"femopt",true);
end;

function solo()
	return (getPlayers()[2] == nil);
end;
function femaleOnly()
	return (getRolePlayers("male")[1] == nil);
end;
function maleOnly()
	return (getRolePlayers("female")[1] == nil);
end;
function duo()
	return (not solo() and (getPlayers()[3] == nil)); 
end;
		
--reward_id: a string that indicates the privateVar that has to be ~=true if a reward is to be received
function addPlayersExperience(xp, reward_id)
	local players = getPlayers();
	local i,player;
	for i,player in ipairs(players) do
		if(getPlayerPrivateVar(player,reward_id)~=true)then
			set(player,"experience",get(player,"experience") + xp);
			setPlayerPrivateVar(player,reward_id,true);
		end;
	end;
end;

function addPlayersGold(gold, players)
	players = players or getPlayers();
	local i,player;
	for i,player in ipairs(players) do
		set(player,"gold",get(player,"gold") + gold);
	end;
end;
--counts the number of players in the game
function getPlayerNumber()
	local players = getPlayers()
	local count = 0
	for i,player in ipairs(players) do
		count = count + 1
	end
	return count
end;