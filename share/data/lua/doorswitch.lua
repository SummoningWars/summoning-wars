function door_toggle(door)
	if (type(door) == "string") then
		door = getObjectByNameRef(door);
	end;
	insertTrigger("object_use");
	addTriggerVariable("used_object",door);
	addTriggerVariable("_id",door);
	addTriggerVariable("lock_override",1);
end;

function door_setopen(door, open)
	if (type(door) == "string") then
		door = getObjectByNameRef(door);
	end;
	if ( open ~= getScriptObjectFlag(door,'open')) then
		door_toggle(door)
	end;
end;