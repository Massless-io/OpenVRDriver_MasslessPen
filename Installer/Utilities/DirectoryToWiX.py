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
ThisVariant = "OpenVRDriver"
end_component = "</Component>"
start_component = Template('<Component Id="$variant$id" Guid="$guid" Win64="no">')
#RegkeyLine = Template(' <RegistryValue Root="HKCU" Key="Software/Massless/Studio" Name="$name" Value="$value" Type="string" KeyPath="yes" />')
file_line = Template('<File Id="$variant$fileid" Name="$name" Source="..\\driver_massless\\Output\\driver\\$path$file" />')
directory_line = Template('<Directory Id="${variant}_$idname$unique" Name="$name">')
#RemoveDirectoryLine = Template('<RemoveFolder Id="$id" Directory="$directory" On="uninstall" />')
feature_line = Template('<ComponentRef Id="$variant$id"/>\n')
feature_file = '../../../Installer/openvrdriver_features.wxi'
output_file = '../../../Installer/openvrdriver_directories.wxi'
count = 0
working_dir = '..\\..\\driver_massless\\Output\\driver\\'
os.chdir(working_dir)
print(os.listdir())
with open(feature_file,"w") as featurefile:
    with open(output_file,'w') as outputfile:  
        outputfile.write('<?xml version="1.0" encoding="utf-8"?>\n')
        outputfile.write('<Include>\n')
        featurefile.write('<?xml version="1.0" encoding="utf-8"?>\n')
        featurefile.write('<Include>\n')
        directory=""
        #path_to_convert = Template('../../MASSLESS-Showcase/Builds/Massless Showcase $variant/Massless Showcase ${variant}_Data')
        #print(path_to_convert.substitute(variant=ThisVariant))
        FullTree=["StudioLauncherStaging"]
        UseEndDirectory=False
        RemoveCurrentDirectoryLines = ['<RemoveFolder Id="REMOVECOMPANYFOLDER" Directory="COMPANYFOLDER" On="uninstall" />',
                                       '<RemoveFolder Id="REMOVESTUDIOFOLDER" Directory="STUDIO" On="uninstall" />']
                
        UniqueCount = 0
        
        for i, walkdata in enumerate(os.walk('massless')):
            dirpath, dirs, files = walkdata
            print ("Processing directory: " + dirpath)
            # Append the subdirectories to keep track of where we are in the tree
            if len(dirs)>0:
                FullTree.append(copy.deepcopy(dirs))
                UseEndDirectory=False
            else:
                UseEndDirectory=True

            #print ("FullTree: " + str(FullTree))
            # Start the directory
            # Split the dirpath to only the current directory
            SplitPath = dirpath.split("\\")
            if len(SplitPath)>1:
                CurrentDirectoryName = SplitPath[len(SplitPath)-1]
                CurrentDirectoryId = "_".join(CurrentDirectoryName.split(' '))
                outputfile.write(directory_line.substitute(idname=CurrentDirectoryId,name=CurrentDirectoryName,unique=str(i), variant=ThisVariant)+"\n")
                RemovalId = CurrentDirectoryId + 'FOLDERREMOVAL' + str(i)
                if not RemovalId[0].isalpha():
                    RemovalId = 'n' + RemovalId
                #RemoveCurrentDirectoryLines.append(RemoveDirectoryLine.substitute(id=RemovalId,directory=ThisVariant + '_' + CurrentDirectoryId + str(i)))
            
            # Process all its files
            for file in files:
                #print (dirpath + "\\" + file)
                id = "".join(file.split('.')).lower()
                id = "".join(id.split(' ')).lower()
                id = "_".join(id.split('-')).lower()
                featurefile.write(feature_line.substitute(id=id+str(i), variant=ThisVariant))
                fileid = id + "file"
                if len(dirpath) > 1:
                    path = dirpath + "\\"
                else:
                    path = ""
                UniqueCount+=1
                line1 = start_component.substitute(id=id+str(i), guid=str(uuid.uuid4()).upper(), variant=ThisVariant)
                outputfile.write(line1 + "\n")
                #line2 = RegkeyLine.substitute(name=id,value=id.upper())
                #outputfile.write(line2 + "\n")
                UniqueCount+=1
                line3 = file_line.substitute(fileid=fileid+str(i), name=file, path=path, file=file, variant=ThisVariant)
                outputfile.write(line3 + "\n")
                #while len(RemoveCurrentDirectoryLines)>0:
                    #outputfile.write(RemoveCurrentDirectoryLines.pop() + "\n")
                    #print("Not adding remove directory lines")
                outputfile.write(end_component + "\n")
                count = count + 1
                
            
            # Process the end of the directory
            #print( "Check if we need to end the directory")
            while UseEndDirectory:
                CurrentDepth=len(FullTree)-1
                #print("Current depth: "+str(CurrentDepth))
                if CurrentDepth < 1:
                    UseEndDirectory=False
                    #print("Ending with final <'Directory>")
                    #outputfile.write('</Directory>\n')
                else:
                    del FullTree[CurrentDepth][0]
                    #print("Deleted a folder")
                    outputfile.write('</Directory>\n')
                    #print ("********** </Directory>")
                    #print("Length of remaining" + str(len(FullTree[CurrentDepth])))
                    if len(FullTree[CurrentDepth]) > 0:
                        UseEndDirectory=False
                    else:
                        del FullTree[CurrentDepth]
                        
                        
        outputfile.write('</Include>')
        featurefile.write('</Include>')
            
print("Output complete, " + str(count) + " files processed.")
print("You will need to individually copy and paste the components for one directory together into your destination .wxi file")
print("PLEASE ENSURE: That this is version tracked and you never overwrite any GUID! Only use this to add new files in the structure, or to add a new directory structure.")
