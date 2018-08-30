//
//  Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//

#ifndef diskinfo_hpp
#define diskinfo_hpp

#include "app.h"



class DiskInfo : public Object, public ISerializeToVariant
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
	string diskUrl();
	string imageUrl();

    // ISerializableToVariant
    void fromVariant(const variant& v) override;
    void toVariant(variant& v) override;


protected:
	string fileToUrl(string filename);

	
};



class Game : public Object, public ISerializeToVariant {
public:
	string _title;
	vector<variant> _publishers;
	vector<ObjPtr<DiskInfo>> _diskInfos;
	
    // ISerializeToVariant
    void fromVariant(const variant& v) override;
    void toVariant(variant& v) override;

	//- (id)initAsLocalDisk:(NSString*)localFilePath fileHash:(NSString*)fileHash diskInfo:(DiskInfo*)diskInfo;
	DiskInfo* defaultDiskInfo();
    

};

#endif
