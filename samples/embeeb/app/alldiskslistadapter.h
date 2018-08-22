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
    int getSectionCount() override;
    int getItemCount(int section) override;
    string getSectionTitle(int section) override;
    Object* getItem(LISTINDEX index) override;
    void handleJson(const Variant& json) override;

    map<char32_t, vector<DisksListItem*>> _sectionMap;
    vector<map<char32_t, vector<DisksListItem*>>::iterator> _sections;

};


