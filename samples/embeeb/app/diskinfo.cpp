//
//  diskinfo.cpp
//  emBeeb
//
//  Copyright © 2016 Sandcastle Software Ltd. All rights reserved.
//

#include "diskinfo.h"



//NSString* kLoadDiskNotification = @"kLoadDiskNotification";

//extern NSString* s_docsDir;


Game::Game(JsonObject* json) {
	_json = json;
	_title = json->getString("t");
	_publishers = json->getStringArray("p");
	_diskInfos = json->getObjectArray<DiskInfo>("m");
	for (int i=0 ; i<_diskInfos.size() ; i++) {
		DiskInfo* disk = _diskInfos.at(i);
        if (!disk->_title.length()) {
            disk->_title = _title;
        }
		disk->_game = this;
	}
}

Game::Game(const VariantMap& map) {
    _title = (string)map.get("t");
    assert(0); // time to uncomment the lines below
    //_publishers = map->getStringArray("p");
    //_diskInfos = json->getObjectArray<DiskInfo>("m");
    //[self.diskInfos makeObjectsPerformSelector:@selector(setGame:) withObject:self];
}
void Game::writeSelfToVariantMap(VariantMap& map) {
    map.set("t", _title);
    assert(0); // time to uncomment the lines below
    //map->setStringArray("p", _publishers);
    //map->setObjectArray<DiskInfo>("m", _diskInfos);

}


/*- (id)initAsLocalDisk:(NSString*)localFilePath fileHash:(NSString*)fileHash diskInfo:(DiskInfo*)diskInfo {
	if ((self = (diskInfo ? [NSKeyedUnarchiver unarchiveObjectWithData:[NSKeyedArchiver archivedDataWithRootObject:diskInfo.game]] : [super init]))) {
		DiskInfo* localDiskInfo = nil;
		if (diskInfo) {
			localDiskInfo = [NSKeyedUnarchiver unarchiveObjectWithData:[NSKeyedArchiver archivedDataWithRootObject:diskInfo]];
		} else {
			_diskInfos = [NSMutableArray new];
			localDiskInfo = [[DiskInfo alloc] init];
			localDiskInfo.title = self.title = [localFilePath substringFromIndex:s_docsDir.length+1];
			localDiskInfo.mediaFileHash = fileHash;
		}
		localDiskInfo.localFilePath = localFilePath;
		localDiskInfo.game = self;
		[self.diskInfos removeAllObjects];
		[self.diskInfos addObject:localDiskInfo];
		
	}
	return self;
}*/

DiskInfo* Game::defaultDiskInfo() {
	for (int i=0 ; i<_diskInfos.size() ; i++) {
		DiskInfo* disk = _diskInfos.at(i)._obj;
		if (0==disk->_platform.compare("elk")) {
			continue;
		}
		if (0==disk->_format.compare("e00")) {
			continue;
		}
		return disk;
	}
	if (_diskInfos.size() > 0) {
		return _diskInfos.at(0);
	}
	return NULL;
}





DiskInfo::DiskInfo() {
}
DiskInfo::DiskInfo(JsonObject* json) {
	_title = json->getString("t");
	_publisher = json->getString("p");
	_mediaFilename = json->getString("u");
	_mediaFileHash = json->getString("h");
	_imageFilename = json->getString("i");
	_platform = json->getString("pl");
	_format = json->getString("f");
	_qualifiers = json->getString("q");
	_version = json->getFloat("v");
}


DiskInfo::DiskInfo(const VariantMap& map) {
    _title = (string)map.get("t");
    _publisher = (string)map.get("p");
    _mediaFilename = (string)map.get("u");
    _mediaFileHash = (string)map.get("h");
    _imageFilename = (string)map.get("i");
    _platform = (string)map.get("pl");
    _format = (string)map.get("f");
    _qualifiers = (string)map.get("q");
    _version = map.get("v");
    _localFilePath = (string)map.get("lfp");
}

void DiskInfo::writeSelfToVariantMap(VariantMap& map) {
    map.set("t", _title);
    map.set("p", _publisher);
    map.set("u", _mediaFilename);
    map.set("h", _mediaFileHash);
    map.set("i", _imageFilename);
    map.set("pl", _platform);
    map.set("f", _format);
    map.set("q", _qualifiers);
	map.set("v", _version);
    map.set("lfp", _localFilePath);
}

string DiskInfo::fileToUrl(string filename) {
	string url =  urlEncode(_game->_title);
	url.append("/");
	url.append(urlEncode(filename));
	url.insert(0, "http://www.ibeeb.co.uk/games/");
	return url;
}

string DiskInfo::diskUrl() {
	/*if (self.localFilePath) {
		return [NSURL fileURLWithPath:self.localFilePath].absoluteString;
	}*/
	return fileToUrl(_mediaFilename);
}

string DiskInfo::imageUrl() {
	//NSString* imageFilename = self.imageFilename;
	if (_imageFilename.length() == 0) return "";
	return fileToUrl(_imageFilename);
}

/*- (BOOL)isEqual:(id)object {
	DiskInfo* other = (DiskInfo*)object;
	return [self.mediaFileHash isEqual:other.mediaFileHash] || (!self.mediaFileHash && !other.mediaFileHash);
}*/


