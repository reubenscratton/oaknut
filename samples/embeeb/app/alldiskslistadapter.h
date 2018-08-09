//
//  emBeeb
//
//  Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//

#include "diskslistadapter.h"


class AllDisksListAdapter : public DisksListAdapter {
public:
    AllDisksListAdapter(string srcfile);

    // Overrides
    virtual int getSectionCount();
    virtual int getItemCount(int section);
    virtual string getSectionTitle(int section);
    
    map<char32_t, vector<DisksListItem*>> _sectionMap;
    vector<map<char32_t, vector<DisksListItem*>>::iterator> _sections;

    virtual void onUrlRequestLoad(URLData* data);
    virtual Object* getItem(LISTINDEX index);
};


