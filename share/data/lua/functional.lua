
-- wendet auf alle Elemente der Tabelle die Funktion filterfunc an
-- gibt eine Tabelle aus, die nur die Elemente enthaelt, fuer die filterfunc true ausgegeben hat
function filter(tab, filterfunc)
		local ret = {};
		local l =1;
		for i,val in ipairs(tab) do
			if ( filterfunc(val)) then
				ret[l] = val;
				l = l+1;
			end;
		end;
		return ret;
end;


-- wendet auf alle Elemente der Tabelle tab die Funktion func an
function forEach(tab,func)
	for key,val in pairs(tab) do
		func(val)
	end;
end;

-- wendet auf alle Elemente der Tabelle tab die Funktion func an
-- gibt eine Tabelle mit den selben Schluesseln aus, die die Funktionsergebnisse enthaelt
function map(tab,func)
	local ret = {};
	for key,val in pairs(tab) do
		ret[key] = func(val)
	end;
	return ret;
end;

