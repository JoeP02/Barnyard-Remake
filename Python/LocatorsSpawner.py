#!/usr/bin/python
# -*- coding: utf-8 -*-
import unreal

file = open('C:\\barn_locators.txt')
lines = file.readlines()

locatorClass = unreal.EditorAssetLibrary.load_blueprint_class('/Game/Barnyard/Blueprints/BP_Locator')
tree = unreal.EditorAssetLibrary.load_blueprint_class('/Game/Barnyard/Blueprints/Props/tree')
treelittle = unreal.EditorAssetLibrary.load_blueprint_class('/Game/Barnyard/Blueprints/Props/treelittle')
treemidsize = unreal.EditorAssetLibrary.load_blueprint_class('/Game/Barnyard/Blueprints/Props/treemidsize')
treemidleaf = unreal.EditorAssetLibrary.load_blueprint_class('/Game/Barnyard/Blueprints/Props/treemidleaf')
treetwiggy = unreal.EditorAssetLibrary.load_blueprint_class('/Game/Barnyard/Blueprints/Props/treetwiggy')
leafyplant = unreal.EditorAssetLibrary.load_blueprint_class('/Game/Barnyard/Blueprints/Props/leafyplant')
treewall = unreal.EditorAssetLibrary.load_blueprint_class('/Game/Barnyard/Blueprints/Props/treewall')

for line in lines:
    if line.startswith('@locator'):
        arr = line.split(' ')
        locName = arr[1]
        c = None
            
        quatX = float(arr[2].replace(',', '.')) * -1
        quatY = float(arr[3].replace(',', '.'))
        quatZ = float(arr[4].replace(',', '.')) * -1
        quatW = float(arr[5].replace(',', '.'))
        posX = float(arr[6].replace(',', '.')) * 100
        posY = float(arr[7].replace(',', '.')) * -100
        posZ = float(arr[8].replace(',', '.')) * 100
        flag = int(arr[9])
        
        quat = unreal.Quat(quatX, quatY, quatZ, quatW).rotator()
        
        if locName.startswith("tree"):
            c = tree
            
            if locName.startswith("treelittle"):
                c = treelittle
            elif locName.startswith("treemidleaf"):
                c = treemidleaf
            elif locName.startswith("treemidsize"):
                c = treemidsize 
            elif locName.startswith("treetwiggy"):
                c = treetwiggy
                
            if locName.startswith("treewall"):
                c = treewall
            
        elif (locName.startswith("bullrush")) or (locName.startswith("fnc")) or (locName.startswith("rockpeb")) or (locName.startswith("rock")) or (locName.startswith("busha")) or (locName.startswith("sandypeb")) or (locName.startswith("reeds")) or (locName.startswith("flwrs")):
            c = unreal.EditorAssetLibrary.load_blueprint_class('/Game/Barnyard/Blueprints/Props/' + locName)
        elif locName.startswith("leafyplant"):
            c = leafyplant
        elif (locName.startswith("mush_")):
            c = unreal.EditorAssetLibrary.load_blueprint_class('/Game/Barnyard/Blueprints/Props/' + locName.split("_")[0] + "_" + locName.split("_")[1])

        if c == None:
            try:
                c = unreal.EditorAssetLibrary.load_blueprint_class('/Game/Barnyard/Blueprints/Props/' + locName.split("_")[0])
            except:
                continue
        
        if c == None:
            continue
            
        asset = unreal.EditorLevelLibrary.spawn_actor_from_class(c, unreal.Vector(posX, posY, posZ), quat)
        asset.tags = [locName]
        asset.set_actor_label(locName)
        asset.set_folder_path("Locators")