-- locations: berechnete Orte (Tabelle)
-- center Zentrum des Halbkreises
function generateArc(locations, center, radius, number, arcangle, angle)
	angle = angle or 0;
	arcangle = arcangle or 120;
	number = number or 1;
	local n = table.getn(locations);
	local inc = arcangle / (number);
	local angl = angle - 0.5*arcangle + inc*0.5;
	local i;
	for i=0,number-1 do
		locations[n+1] = {center[1] + math.cos(angl*math.pi/180)*radius, center[2] + math.sin(angl*math.pi/180)*radius};
		angl = angl + inc;
		n = n+1;
	end;
end;

-- erzeugt Punkt in einem Rechteck
-- center - das Zentrum
-- number Anzahl der Punkte
-- dim: Anzahl der Punkt in der ersten Reihe
-- angle: Drehwinkel
function generateRect(locations, center, number, dim, angle, dist)
	angle = angle or 0
	dist = dist or 1
	angle = angle*math.pi/180
	local dim2 = math.ceil(number/dim);
	local i,j
	local x,y;
	local jmax;
	local n = table.getn(locations);
	for i=1,dim2 do
		x = ((dim2+1)/2 - i)*dist;
		jmax = math.min(number,dim);
		for j=1,jmax do
			y = (j-(jmax+1)/2)*dist;
			locations[n+1] = {center[1]+x*math.cos(angle)-y*math.sin(angle),center[2]+ x*math.sin(angle)+y*math.cos(angle)}
			n = n+1;
		end;
		number = number - dim;
	end;
end;

--setzt den uebergebenen Vector auf den Mittelpunkt des Tiles, zu dem er gehoert
function getNextTileCenter(vec)
	vec[1] = math.floor(vec[1]/8) * 8 + 4;
	vec[2] = math.floor(vec[2]/8) * 8 + 4;
end;