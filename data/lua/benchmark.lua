function printOgreStats()
	local stats = getOgreStatistics();
	writeLog("info", "average FPS:    "..stats["avgFPS"]);
	writeLog("info", "worst FPS:      "..stats["worstFPS"]);
	writeLog("info", "max. frametime: "..stats["worstFrameTime"]);
	writeLog("info", "triangles:      "..stats["triangleCount"]);
	writeLog("info", "batches:        "..stats["batchCount"]);
	writeLog("info","------------------------");
	
	printMessage("average FPS:   "..stats["avgFPS"]);
	printMessage("worst FPS:     "..stats["worstFPS"]);
	printMessage("max frametime: "..stats["worstFrameTime"]);
	printMessage("triangles:     "..stats["triangleCount"]);
	printMessage("batches:       "..stats["batchCount"]);
	printMessage("------------------------");
end;

function startBenchmark()
	setOption("cap_fps","false");
	jump("benchmarkBox","locCenter");
end;

function benchmark(duration, repetitions, first)
	duration = duration or 5000;
	repetitions = repetitions or 1;
	first = first or 1;
	
	if (first == 2) then
		printOgreStats();
	end;
	
	clearOgreStatistics();
	if (repetitions > 0) then
		-- delayed recursive call
		timedExecute("benchmark("..duration..","..(repetitions-1)..",2);", duration);
	end;
end;