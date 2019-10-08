function vecrotate(vec,angle)
	local rotvec={0,0};
	angle = angle*math.pi/180;
	rotvec[1] = math.cos(angle)*vec[1] - math.sin(angle)*vec[2];
	rotvec[2] = math.sin(angle)*vec[1] + math.cos(angle)*vec[2];
	vec[1] = rotvec[1];
	vec[2] = rotvec[2];
end;

function vecadd(vec1,vec2)
	vec1[1] = vec1[1] + vec2[1];
	vec1[2] = vec1[2] + vec2[2];
end;

function vecsub(vec1,vec2)
	vec1[1] = vec1[1] - vec2[1];
	vec1[2] = vec1[2] - vec2[2];
end;

function vecmul(vec1,mul)
	vec1[1] = vec1[1] * mul;
	vec1[2] = vec1[2] * mul;
end;

function vecnormal(vec)
	local l = math.sqrt(vec[1]*vec[1] + vec[2]*vec[2]);
	vec[1] = vec[1] / l;
	vec[2] = vec[2] / l;
end

