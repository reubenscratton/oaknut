//
//  Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//

#include "snapshot.h"


Snapshot::Snapshot() {
}

// ISerializeToVariantMap
Snapshot::Snapshot(const VariantMap& v) {
    _diskInfo = v.get<DiskInfo>("diskInfo");
    _data = (ByteBuffer*)v["data"];
    _timestamp = v["timestamp"];
    _thumbnail = v.get<Bitmap>("thumbnail");
    _label = (string)v["label"];
    _controllerId = (string)v["controllerId"];

}
void Snapshot::writeSelfToVariantMap(VariantMap& v) {
    v["diskInfo"] = _diskInfo._obj;
    v["data"] = _data;
    v["timestamp"] = _timestamp;
    v["thumbnail"] = _thumbnail._obj;
    v["label"] = _label;
    v["controllerId"] = _controllerId;
}


void Snapshot::updateWithData(ByteBuffer* data, Bitmap* thumbnail, string controllerId) {
	_timestamp = app.currentMillis();
	_thumbnail = thumbnail;
	_controllerId = controllerId;
    _data = data;
	
	/*
	// Update the index file
	[s_snapshots removeObject:self];
	[s_snapshots insertObject:self atIndex:0];
	[Snapshot saveIndex];*/
}


