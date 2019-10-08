function gettext(text,substrings)
	if (substrings == nil) then
		return text;
	end;
	
	substrings[0] = text;
	return substrings
end;

-- underscore is the gettext function as well...
_ = gettext;