
-- puts an object in the NOCOLLISION-layer and sets it 20 units under the earth, meaning it will be not visible anymore
function getObjectOutOfWay(obj)
	set(obj,"layer","nocollision");
	set(obj,"height",-20);
end;
-- gets an object back on the normal floor
function getObjectOnFloor(obj)
	set(obj,"layer","normal");
	set(obj,"height",0);
end;

--rotates an object
--obj: the id of the rotated object
--degrees: the number of degrees the object is to be rotated
--step: amount of degrees per step; consider sign
--interval: the speed. interval in ms/step
function rotate(obj, degrees, step, interval)
	if(degrees==0)then
		return;
	else
		local angle = get(obj,"angle") + step;
		set(obj,"angle",angle);
		timedExecute('rotate('..obj..','.. degrees - 1 ..','..step..','..interval..')',interval);
	end;
end;

--set(obj,"usable",false)

-- tutorial specific function to determine what constellation the experiment using the energy source has
-- return 0 for ice, 1 for air, 2 for fire, 3 for puzzle solved and 4 for nothing
function experimentStatus()
	local ice = getObjectByNameRef("experimentIce")
	local fire = getObjectByNameRef("experimentFire")
	local air = getObjectByNameRef("experimentAir")
	
	local iceStat = scriptobjectvar[ice]["color"];
	local fireStat = scriptobjectvar[fire]["color"];
	local airStat = scriptobjectvar[air]["color"];
	
	if((iceStat==0) and (airStat==1) and (fireStat==2))then
		-- the combination is right
		return 3
	elseif((iceStat==airStat) or (iceStat==fireStat))then
		--one element overweights the others
		return iceStat
	elseif(airStat==fireStat)then
		--one element overweights the others
		return airStat
	else
		--nothing happens
		return 4
	end
end;


