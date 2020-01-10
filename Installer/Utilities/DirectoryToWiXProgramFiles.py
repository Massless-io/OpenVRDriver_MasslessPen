# -*- coding: utf-8 -*-
"""
Python script to process the directory structure into a wix file.

Designed to be run from the top level of the directory structure needing converting.
Will do the entire directory structure as one file, and can files can be directly copied over.
Could be used as a template, if re-generating, check version control and try not to overwrite the GUIDs of each item.
"""

import os
import uuid
from string import Template
import copy
ThisVariant = "StudioLauncher"
end_component = "</Component>"
start_component = Template('<Component Id="$variant$id" Guid="$guid" Win64="yes">')
file_line = Template('<File Id="$variant$fileid" Name="$name" Source="StudioLauncherStaging/$path$file" KeyPath="yes" />')
directory_line = Template('<Directory Id="${variant}SCOD_$name" Name="$name">')
feature_line = Template('<ComponentRef Id="$variant$id"/>\n')
feature_file = 'studio_features.wxi'
output_file = 'studio_directories.wxi'
count = 0
working_dir = '..'
os.chdir(working_dir)
with open(feature_file,"w") as featurefile:
    with open(output_file,'w') as outputfile:  
        outputfile.write('<?xml version="1.0" encoding="utf-8"?>\n')
        outputfile.write('<Include>\n')
        featurefile.write('<?xml version="1.0" encoding="utf-8"?>\n')
        featurefile.write('<Include>\n')
        currentline = Template('<Directory Id="${variant}SCO_DATAFOLDER" Name="Massless ${variant}_data">\n')
        #outputfile.write(currentline.substitute(variant=ThisVariant))
        #print(currentline.substitute(variant=ThisVariant))
        directory=""
        #path_to_convert = Template('../../MASSLESS-Showcase/Builds/Massless Showcase $variant/Massless Showcase ${variant}_Data')
        #print(path_to_convert.substitute(variant=ThisVariant))
        FullTree=["StudioLauncherStaging"]
        UseEndDirectory=False
                
        for dirpath, dirs, files in os.walk('StudioLauncherStaging'):
            print ("Processing directory: " + dirpath)
            # Append the subdirectories to keep track of where we are in the tree
            if len(dirs)>0:
                FullTree.append(copy.deepcopy(dirs))
                UseEndDirectory=False
            else:
                UseEndDirectory=True

            print ("FullTree: " + str(FullTree))
            # Start the directory
            outputfile.write(directory_line.substitute(name=dirpath, variant=ThisVariant))
            
            # Process all its files
            for file in files:
                #print (dirpath + "\\" + file)
                id = "".join(file.split('.')).lower()
                id = "".join(id.split(' ')).lower()
                id = "_".join(id.split('-')).lower()
                featurefile.write(feature_line.substitute(id=id, variant=ThisVariant))
                fileid = id + "file"
                if len(directory) > 1:
                    path = directory + "/"
                else:
                    path = ""
                line1 = start_component.substitute(id=id, guid=str(uuid.uuid4()).upper(), variant=ThisVariant)
                outputfile.write(line1 + "\n")
                line2 = file_line.substitute(fileid=fileid, name=file, path=path, file=file, variant=ThisVariant)
                outputfile.write(line2 + "\n")
                outputfile.write(end_component + "\n")
                count = count + 1
            
            # Process the end of the directory
            print( "Check if we need to end the directory")
            while UseEndDirectory:
                CurrentDepth=len(FullTree)-1
                print("Current depth: "+str(CurrentDepth))
                if CurrentDepth < 1:
                    UseEndDirectory=False
                    print("Ending with final <'Directory>")
                    outputfile.write('</Directory>\n')
                else:
                    del FullTree[CurrentDepth][0]
                    print("Deleted a folder")
                    outputfile.write('</Directory>\n')
                    print ("********** </Directory>")
                    print("Length of remaining" + str(len(FullTree[CurrentDepth])))
                    if len(FullTree[CurrentDepth]) > 0:
                        UseEndDirectory=False
                    else:
                        del FullTree[CurrentDepth]
                
                
        #outputfile.write('</Directory>\n</Directory>\n')
        currentline = Template('<Component Id="${variant}UnityPlayer" Guid="$guid" Win64="yes">\n')
        outputfile.write(currentline.substitute(variant=ThisVariant,guid=str(uuid.uuid4()).upper()))
        currentline = Template('<File Id="${variant}UnityPlayerDLL" Name="UnityPlayer.dll" Source="StudioLauncherStaging/UnityPlayer.dll" KeyPath="yes" />\n')
        outputfile.write(currentline.substitute(variant=ThisVariant))
        outputfile.write('</Component>\n')
        featurefile.write(feature_line.substitute(id="UnityPlayer", variant=ThisVariant))
        currentline = Template('<Component Id="${variant}MasslessStudioLauncher" Guid="$guid" Win64="yes">\n')
        outputfile.write(currentline.substitute(guid=str(uuid.uuid4()).upper(), variant=ThisVariant))
        currentline = Template('<File Id="MasslessStudioLauncherexe$variant" Name="Massless Studio.exe" Source="StudioLauncherStaging/StudioLauncher.exe" KeyPath="yes">\n')
        outputfile.write(currentline.substitute(variant=ThisVariant))
        currentline = Template('<Shortcut Id="MasslessStudioStartMenuShortcut" Name="Massless Studio"\n')
        outputfile.write(currentline.substitute(variant=ThisVariant))
        currentline = Template('WorkingDirectory="$variant" Icon="MasslessIcon.ico" IconIndex="0"\n')
        outputfile.write(currentline.substitute(variant=ThisVariant))
        outputfile.write('Directory="ApplicationProgramsFolder" Advertise="yes" />\n')
        outputfile.write('</File>\n')
        outputfile.write('</Component>\n')
        featurefile.write(feature_line.substitute(id="MasslessStudioLauncher", variant=ThisVariant))
        outputfile.write('</Include>')
        featurefile.write('</Include>')
            
print("Output complete, " + str(count) + " files processed.")
print("You will need to individually copy and paste the components for one directory together into your destination .wxi file")
print("PLEASE ENSURE: That this is version tracked and you never overwrite any GUID! Only use this to add new files in the structure, or to add a new directory structure.")
