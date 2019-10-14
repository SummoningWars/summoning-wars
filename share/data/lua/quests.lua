function printTable(tab,name,indent)
	local key,value;
	if (indent==nil) then indent=0 end;
	if (name == nil ) then name=""; end;
	print(string.rep (" ", indent).."<"..name..">");
	for key,value in pairs(tab) do
		if (type(value) == "table") then
			printTable(value,key,indent+4)
		else
			print(string.rep (" ", indent+4)..tostring(key).." = "..tostring(value));
		end;
	end;
	print(string.rep (" ", indent).."</"..name..">");
end;

function eval(str)
   return assert(loadstring(str))()
end

function printquestvars(tab)
	for key,value in pairs(tab) do
		printTable(value,key);
	end;
end;


function deepcopy(object)
    local lookup_table = {}
    local function _copy(object)
        if type(object) ~= "table" then
            return object
        elseif lookup_table[object] then
            return lookup_table[object]
        end
        local new_table = {}
        lookup_table[object] = new_table
        for index, value in pairs(object) do
            new_table[_copy(index)] = _copy(value)
        end
        return setmetatable(new_table, getmetatable(object))
    end
    return _copy(object)
end


function removePlayer(id)
	playervars[id] = nil;
end;

function addPlayer(id)
	playervars[id] = {};
end;

function writeTable(tab,name,data)
	local key,value,sep
	sep="";
	data.str=data.str..name.."={";
	for key,value in pairs(tab) do
		if (type(value) == "table") then
			data.str = data.str..sep;
			writeTable(value,key,data);
		elseif type(value) == "string" then
			data.str = data.str..sep..key.."='"..value.."'";
		elseif (value ~= nil) then
			data.str = data.str..sep..key.."="..tostring(value);
		end;
		sep=",";
	end;
	data.str = data.str.."}";
end;

function writeSavegame() 
	local quest,qname,data; 
	data = {};
	data.str=""
	for qname,quest in pairs(quests) do 
		writeTable(quest,qname,data);
		writeString(data.str);
		--print(data.str);
		data.str = "";
		writeNewline();
	end;
end;

function writeQuest(quest,qname,pid)
	local data={str=""};
	writeTable(quest,qname,data);
	data.str = data.str..";quests."..qname.."="..qname..";"
	writeUpdateString(pid,data.str);
	--print("setting quest: "..data.str);
	playervars[pid][qname] = deepcopy(quest);
end;


function initPlayerVars(id)
	local pid=id;
	local playertab, reftab;
	local started, finish_fail, ref_finish_fail
	local data={},qname,quest
	for qname,quest in pairs(quests) do
		-- private Variablen des Spielers
		reftab = playervars[pid][qname];

		-- Vergleich der privaten Variable mit den gesetzten
		started = quest.started;
		finish_fail = quest.finished or quest.failed;
		if (started and not finish_fail) then
			if (reftab == nil) then
				-- Quest auf Clientseite unbekannt
				-- genau dann uebertragen, wenn es gestartet, aber nicht beendet ist
				writeQuest(quest,qname,pid);
			else
				ref_finish_fail = reftab.finished or reftab.failed;
				if (not ref_finish_fail) then
					writeQuest(quest,qname,pid)
				end;
			end;
		end;
	end;
end;

function updatePlayerVariable(var,refvar,prefix,lplayers)
	local value,key,refval;
	local pid,pdata,data,str;
	for key,value in pairs(var) do
		refval = refvar[key];
		if (type(value) == "table") then
			-- Tabelle
			if (refval == nil) then
				refvar[key] = deepcopy(value);
				data = {str=""};
				writeTable(value,prefix.."."..key,data);
				data.str =data.str..";";
				--print("new subtable: "..data.str);
				for pid,pdata in pairs(lplayers) do
					writeUpdateString(pid,data.str);
					eval(pdata..data.str);
				end;
			else
				updatePlayerVariable(value,refval,prefix.."."..key,lplayers);
			end;
		else
			-- keine Tabelle
			if (refval ~= value) then
				refvar[key] = value;
				if (type(value) == "string") then
					str = prefix.."."..key.."='"..tostring(value).."';";
				else
					str = prefix.."."..key.."="..tostring(value)..";";
				end;
				--print("changed variable: "..str);
				for pid,pdata in pairs(lplayers) do
					writeUpdateString(pid,str);
					eval(pdata..str);
				end;
			end;
		end;
	end;
end;


function updatePlayerVars(loc_pid)
	local data={},qname,quest;
	local started, old_started, finish_fail, old_finish_fail, l_started, l_finish_fail,lplayers={};
	local pid,plvar,pldata;
	local data={},qname,quest,locquest;
	
	-- Schleife ueber die Quests
	for qname,quest in pairs(quests) do
		locquest = playervars[loc_pid][qname];
		if (locquest == nil) then
			locquest = deepcopy(quest);
			playervars[loc_pid][qname] = locquest;
		end;
		
		started = quest.started;
		finish_fail = quest.finished or quest.failed;
		old_started = locquest.started;
		old_finish_fail = locquest.finished or locquest.failed;
		
		if ((started and not finish_fail) or (old_started and not old_finish_fail)) then
			-- Quest ist aktiv (gewesen/geworden)
			-- Schleife ueber die Spieler, testen bei welchen ueberhaupt Daten geaendert werden muessen
			lplayers ={};
			for pid,plvar in pairs(playervars) do
				if (pid ~= loc_pid) then
					pldata = plvar[qname];
					if (pldata == nil) then
						-- Quest beim Client unbekannt
						writeQuest(quest,qname,pid);
					else
						l_started = pldata.started;
						l_finish_fail = pldata.finished or pldata.failed;
						
						if (not l_started) then
							writeQuest(quest,qname,pid);
						elseif (not l_finish_fail) then
							lplayers[pid] = "playervars["..pid.."].";
						end;
					end;
				end;
			end;
			
			-- Variablen aktualisieren
			updatePlayerVariable(quest,locquest,qname,lplayers);
		end;
	end;
end;

function getPlayerPrivateVar(id, varstr)
	local str;
	str ="_ret = playervars["..id.."].private_vars."..varstr;
	eval(str);
	return _ret;
end;

function setPlayerPrivateVar(id,varstr,value)
	local str;
	_ret = value;
	varstr = "private_vars."..varstr;
	str ="playervars["..id.."]."..varstr.." = _ret";
	eval(str);
	local data={str=""};
	data.str ="";
	if (type(value) == "table") then
		writeTable(value,varstr,data);
		data.str = data.str..";";
	elseif (type(value) == "string") then
		data.str = varstr.."='"..value.."';";
	else
		data.str = varstr.."="..tostring(value)..";"
	end;
	writeUpdateString(id,data.str);
end;

