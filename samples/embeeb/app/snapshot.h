//
//  Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//

#include "app.h"
#include "diskinfo.h"


class Snapshot : public Object, public ISerializeToVariant {
public:
    ObjPtr<DiskInfo> _diskInfo;
    string _label;
    TIMESTAMP _timestamp;
    ObjPtr<Bitmap> _thumbnail;
    bytearray _data;
    string _controllerId;

    Snapshot();
    void updateWithData(ByteBuffer* data, Bitmap* thumbnail, string controllerId);

    // ISerializeToVariant
    void fromVariant(const variant& v) override;
    void toVariant(variant& v) override;
};

