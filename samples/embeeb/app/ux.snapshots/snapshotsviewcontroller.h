//
//  Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//

#pragma once
#include "../app.h"
#include "../model/snapshot.h"
#include "../ux.main/beebview.h"

typedef std::function<void(Snapshot*)> SnapshotSelectedDelegate;

class SnapshotsViewController : public ViewController {
public:
    ListView* _listView;
    float _minTopScrollInset;
    vector<sp<Snapshot>> _snapshots;

    
    SnapshotsViewController(Beeb* beeb, BeebView* beebView, DiskInfo* diskInfo, SnapshotSelectedDelegate delegate);
    
    // Overrides
    //virtual void onWillResume();
    //virtual void onDidPause();
    
protected:
    SnapshotSelectedDelegate _delegate;
    bool _showCreateButton;
    Beeb* _beeb;
    BeebView* _beebView;
    DiskInfo* _diskInfo;
    string _controllerId;
    sp<LocalStore> _snapshotStore;
    
    uint32_t saveSnapshot(Snapshot* snapshot);
};


