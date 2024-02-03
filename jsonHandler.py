import json

jsonFile = {}

#name of the json file to load in jsonFile (ext. excluded)
def loadJSON(name):
    global jsonFile

    try:
        file = open("./docs/"+name+".json", "r")
        jsonFile = json.load(file)
        file.close()
        return jsonFile
    except:
        print("Missing "+name+".json")
        return False

def saveJSON(name, contents):
    global jsonFile

    try:
        file = open("./docs/"+name+".json", "w")
        json.dump(contents, file)
        file.close()
        return True
    except:
        print("Some error occured while saving "+name+".json")
        return False
    
def addToJSON(name, contents):
    global jsonFile

    try:
        json, ret = loadJSON(name)
        if(ret == False): return False

        json.update(contents)
        ret = saveJSON(name, json)[1]
        return ret
    
    except:
        return False