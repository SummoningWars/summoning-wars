function listToString(list)
		local i;
		local str="{";
		for i = 1,#list do
			if (i ~= 1) then
				str = str..",";
			end;
			str = str..list[i];
		end;
		return str.."}";
end;

--wandelt eine indizierte Liste in eine Map-aehnliche Darstellung
function listToSet(list)
	local ret = {};
	local i, obj
	for i,obj in ipairs(list) do
		ret[obj] = true;
	end;
	return ret;
end;

function locnameToLocation(loc)
	if (type(loc) == "string") then
		loc = getLocation(loc);
	end;
	return loc;
end;