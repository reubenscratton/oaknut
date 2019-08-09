//
//  diskinfo.cpp
//  emBeeb
//
//  Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//

#include "diskinfo.h"



//NSString* kLoadDiskNotification = @"kLoadDiskNotification";

//extern NSString* s_docsDir;


void Game::fromVariant(const variant& v) {
	_title = v.stringVal("t");
    _publishers = v.arrayRef("p");
    auto diskInfos = v.arrayRef("m");
    for (auto& vdi : diskInfos) {
        DiskInfo* disk = new DiskInfo();
        disk->fromVariant(vdi);
        if (!disk->_title.length()) {
            disk->_title = _title;
        }
        disk->_game = this;
        _diskInfos.push_back(disk);
    }
}

void Game::toVariant(variant& v) {
    v.set("t", _title);
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

void DiskInfo::fromVariant(const variant& v) {
	_title = v.stringVal("t");
	_publisher = v.stringVal("p");
	_mediaFilename = v.stringVal("u");
	_mediaFileHash = v.stringVal("h");
	_imageFilename = v.stringVal("i");
	_platform = v.stringVal("pl");
	_format = v.stringVal("f");
	_qualifiers = v.stringVal("q");
	_version = v.floatVal("v");
    _localFilePath = v.stringVal("lfp");
}

void DiskInfo::toVariant(variant& v) {
    v.set("t", _title);
    v.set("p", _publisher);
    v.set("u", _mediaFilename);
    v.set("h", _mediaFileHash);
    v.set("i", _imageFilename);
    v.set("pl", _platform);
    v.set("f", _format);
    v.set("q", _qualifiers);
	v.set("v", _version);
    v.set("lfp", _localFilePath);
}

string DiskInfo::fileToUrl(string filename) {
	string url = _game->_title.urlEncode();
	url.append("/");
	url.append(filename.urlEncode());
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


