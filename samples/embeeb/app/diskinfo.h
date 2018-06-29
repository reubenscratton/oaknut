//
//  Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//

#ifndef diskinfo_hpp
#define diskinfo_hpp

#include "app.h"



class DiskInfo : public Object, public ISerializeToVariantMap
{
public:
	ObjPtr<class Game> _game;
	string _title;
	string _publisher;
	string _mediaFilename;
	string _mediaFileHash;
	string _imageFilename;
	string _platform;
	string _format;
	float _version;
	string _qualifiers;
	string _localFilePath;
	
    DiskInfo();
	DiskInfo(JsonObject* json);
	string diskUrl();
	string imageUrl();

    // ISerializable
    DiskInfo(const VariantMap& map);
    virtual void writeSelfToVariantMap(VariantMap& map);


protected:
	string fileToUrl(string filename);

	
};



class Game : public Object, public ISerializeToVariantMap {
public:
	ObjPtr<JsonObject> _json;
	string _title;
	vector<string> _publishers;
	vector<ObjPtr<DiskInfo>> _diskInfos;
	
	Game(JsonObject* json);

    // ISerializable
    Game(const VariantMap& map);
    virtual void writeSelfToVariantMap(VariantMap& map);

	//- (id)initAsLocalDisk:(NSString*)localFilePath fileHash:(NSString*)fileHash diskInfo:(DiskInfo*)diskInfo;
	DiskInfo* defaultDiskInfo();
    

};

#endif
