# this script checks for common errors when running and building sumwars for linux
# run this script in your sumwars folder where the executable is

import os.path

#check if executable exists
f = "./sumwars"
if os.path.isfile(f):
  print("Checking for " + f + ": Ok")
else:
  print("Checking for " + f + ": Failure! Please check the location")


#check if plugins.cfg exists
f = "./plugins.cfg"
if os.path.isfile(f):
  print("Checking for " + f + ": Ok")
  
  pluginsfolder = ""
  of = open(f)
  try:
    for line in of:
      if line.startswith("PluginFolder="):
	pluginsfolder = line.replace("PluginFolder=", "").rstrip()
	
	#plugin folder name found, check if it exists
	if os.path.exists(pluginsfolder):
	  print("Checking for " + pluginsfolder + ": Ok")
	else:
	  print("Checking for " + pluginsfolder + ": Failure! Please check the file location")
  finally:
    of.close()
else:
  print("Checking for " + f + ": Failure! Please check the file location")
  
#check if resources.cfg exists
f = "./resources.cfg"
if os.path.isfile(f):
  print("Checking for " + f + ": Ok")
else:
  print("Checking for " + f + ": Failure! Please check the file location")
  
#check if resources folder exists
f = "./resources"
if os.path.exists(f):
  print("Checking for " + f + ": Ok")
else:
  print("Checking for " + f + ": Failure! Please check the path location")
  
#check if data folder exists
f = "./data"
if os.path.exists(f):
  print("Checking for " + f + ": Ok")
else:
  print("Checking for " + f + ": Failure! Please check the path location")
  
#check if save folder exists
f = "./save"
if os.path.exists(f):
  print("Checking for " + f + ": Ok")
else:
  print("Checking for " + f + ": Failure! Please check the path location")