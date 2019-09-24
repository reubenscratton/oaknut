//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#pragma once
#include <oaknut.h>
#include "layout.h"

class BNOptimizedLayout : public Object {
public:
    
    variant _json;
    map<string, BNLayout*> _expandedLayouts;

    vector<string> _content;
    vector<string> _format;
    vector<string> _site;
    vector<string> _modes;

    BNOptimizedLayout(const variant& json);
    BNLayout* layoutForModes(const vector<string>& modes, const variant& incData);

protected:
    void expandJson(variant& json, const variant& inc, variant& incLocal, vector<string> modes);
    void inheritVals(variant& dict, const map<string,variant>& fromDict, const vector<string>& modes);

};


class BNLayoutsGroup : public Object {
public:
    vector<BNOptimizedLayout*> _optimizedLayouts;
    BNOptimizedLayout* _defaultOptimizedLayout;
    variant _incBuffer;
    
    bool isValid();

    BNLayoutsGroup(string layoutsDir);
    
    BNLayout* layoutWithContent(const string& modelId,
                                const string& site,
                                const string& format,
                                const string& orientation);
    
    variant loadJson(const string& layoutsDir, const string& fileName);
    void addFilesFrom(const string& layoutsDir);
    
protected:
    BNOptimizedLayout* chooseLayoutForModelId(const string& modelId,
                                              const string& site,
                                              const string& format,
                                              const vector<string>& modes);

};
