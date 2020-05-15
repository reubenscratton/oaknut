//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "layoutsgroup.h"
#include "../../model/_module.h"
#include "../../policy/policy.h"
//#import "BNStyles.h"
//#import "BNPolicyManager.h"
//#import "BNPolicy.h"



BNOptimizedLayout::BNOptimizedLayout(const variant& json) {
    _json = json;
    _modes = json.stringArrayVal("modes");
    _content = json.stringArrayVal("content");
    _format = json.stringArrayVal("format");
    _site = json.stringArrayVal("site");
}

BNLayout* BNOptimizedLayout::layoutForModes(const vector<string>& modes, const variant& inc) {
	
	// Collapse the modes array to a single string for use as a dictionary key
    string modeset = string::join(modes, ",");
	
	// Do we already have an expanded layout for the given modeset? If so return it
    auto it = _expandedLayouts.find(modeset);
    if (it != _expandedLayouts.end()) {
		return it->second;
	}
	
	// Create the expanded layout using the optimized json and the include data
    variant expandedJson = _json;
    variant incLocal;
    expandJson(expandedJson, inc, incLocal, modes);
    BNLayout* expandedLayout = new BNLayout(expandedJson);
	_expandedLayouts[modeset] = expandedLayout;
	return expandedLayout;
}

void BNOptimizedLayout::expandJson(variant& json, const variant& inc, variant& incLocal, vector<string> modes) {

	// Handle JSON Object
	if (json.isCompound()) {
		
		// Object inheritance. Any JSON object can have a field named "$" whose value names
		// a 'template' JSON object in the include file whose key/value pairs it copies.
		string t;
		while ((t = json.stringVal("$")).length()) { // loop cos templates can themselves be based on templates
			json.unset("$");
			auto& templateJson = inc.compoundRef(t);
			if (templateJson.size()) {
				inheritVals(json, templateJson, modes);
			}
		}
		
		// Check this JSON Object is valid for the given modes.
		auto modesJsonObjIsValidFor = json.stringArrayVal("modes");
        for (auto requiredMode : modesJsonObjIsValidFor) {
            bool excl = requiredMode.hadPrefix("!");
            bool modePresent = std::find(modes.begin(), modes.end(), requiredMode) != modes.end();
            if (excl && modePresent) {
                json.clear();
                return;
            }
            if (!excl && !modePresent) {
                json.clear();
                return;
            }
        }
		
		// Object-local value definitions.
        auto& jsonObj = json.compoundRef();
        auto itr = jsonObj.begin();
        while (itr != jsonObj.end()) {
			if (itr->first.hasPrefix("$") && itr->first.length()>1) {
				incLocal.set(itr->first, itr->second);
				itr = jsonObj.erase(itr);
            } else {
                itr++;
            }
		}
		
		
		// Recurse over fields
        itr = jsonObj.begin();
        while (itr != jsonObj.end()) {

			// Mode-specific sections
			if (itr->first.hasPrefix("mode==") || itr->first.hasPrefix("mode!=")) { // lame
                bool inv = itr->first.charAt(4)=='!';
                string mode = itr->first.substr(6);
                bool modePresent = std::find(modes.begin(), modes.end(), mode) != modes.end();
				if (inv!=modePresent) {
                    variant vals = itr->second;
                    itr = jsonObj.erase(itr);
                    expandJson(vals, inc, incLocal, modes);
                    if (vals.isCompound()) {
                        for (auto& kv2 : vals.compoundRef()) {
                            jsonObj[kv2.first] = kv2.second;
                        }
                        itr = jsonObj.begin();
                        continue;
                    }
                } else {
                    itr = jsonObj.erase(itr);
                }
            } else {
                // Ordinary values
                expandJson(itr->second, inc, incLocal, modes);
                itr++;
            }
		}
	}
	
	// Handle a JSON array
	else if (json.isArray()) {
        vector<variant>& array = json.arrayRef();
		for (int i=0 ; i<array.size() ; i++) {
			variant& value = array[i];
            bool valueWasArray = value.isArray();
            expandJson(value, inc, incLocal, modes);
				
            // If value expanded to nothing, remove the array element
            if (value.isEmpty()) {
                array.erase(array.begin() + i);
                i--;
            }
            
            // Special case! If an array element expanded to another array
            // (and it wasn't one to start with) then unify them.
            else if (!valueWasArray && value.isArray()) {
                vector<variant>& valueArray = value.arrayRef();
                int c = (int)valueArray.size();
                array.insert(array.begin()+i, valueArray.begin(), valueArray.end());
                array.erase(array.begin()+i + c);
                i += c - 1;
            }
		}
	}
	
	// String
	else if (json.isString()) {
        string& str = json.stringRef();
		// Value-replacement: given a value such as "$foo" replace "$foo" with its value from the include file.
		if (str.hasPrefix("$")) {
            variant expandedVal = incLocal.get(str);
            if (expandedVal.isEmpty()) {
                expandedVal = inc.get(str);
            }
			expandJson(expandedVal, inc, incLocal, modes);
            json = expandedVal;
		}
	}
}


void BNOptimizedLayout::inheritVals(variant& json, const map<string,variant>& fromDict, const vector<string>& modes) {
    for (auto& keyval : fromDict) {
        if (keyval.first.hasPrefix("mode==") || keyval.first.hasPrefix("mode!=")) { // lame
            bool inv = keyval.first.charAt(4)=='!';
            string mode=keyval.first.substr(6);
            json.unset(keyval.first);
            bool modePresent = std::find(modes.begin(), modes.end(), mode) != modes.end();
            if (inv!=modePresent) {
                variant modeSpecificVals = keyval.second;
				inheritVals(json, modeSpecificVals.compoundRef(), modes);
			}
		} else {
			if (!json.hasVal(keyval.first)) {
				json.set(keyval.first, keyval.second);
			}
		}
	}
}




/*@interface BNLayoutsGroup ()

@property (nonatomic) NSData* incBuffer;

@end*/



BNLayoutsGroup::BNLayoutsGroup(string layoutsDir) {
    
    // If the layouts file contains a 'layout' subfolder, that's the real layout root folder
    string subfolder = layoutsDir;
    subfolder.append("/layout");
    struct stat buf;
    if (0==stat(subfolder.c_str(), &buf)) {
        layoutsDir = subfolder;
    }

    // Load and store the include file as an NSData
    _incBuffer = loadJson(layoutsDir, "inc.json");
    
    // Load all files
    vector<string> fileNames = File::dir_sync(layoutsDir);
    
    // Iterate the layout files
    for (int i = 0; i < fileNames.size(); i++) {
        string fileName = fileNames[i];
        if (!fileName.hasSuffix(".json")) {
            continue;
        }
        if (fileName == "inc.json") {
            continue;
        }
        variant jsonLayout = loadJson(layoutsDir, fileName);
        if (jsonLayout.isEmpty()) {
            log_warn("Error loading layout: %s", fileName.c_str());
            continue;
        }
        
        BNOptimizedLayout* layout = new BNOptimizedLayout(jsonLayout);
        _optimizedLayouts.push_back(layout);
    }

	// Default layouts
    _defaultOptimizedLayout = chooseLayoutForModelId(BNModelIdTopStories, "", "", {});
}


variant BNLayoutsGroup::loadJson(const string& layoutsDir, const string& fileName) {
    string filePath = layoutsDir;
    filePath.append("/");
    filePath.append(fileName);
    
    variant v = File::load_sync(filePath);
    if (v.isError()) {
        return v;
    }
    auto& data = v.bytearrayRef();
    string str = data.toString();
    return variant::parse(str, PARSEFLAG_JSON);
}




bool BNLayoutsGroup::isValid() {
	// This is pretty primitive validation
	return _defaultOptimizedLayout && _optimizedLayouts.size() > 2;
}

BNLayout* BNLayoutsGroup::layoutWithContent(const string& modelId,
	const string& site, const string& format, const string& orientation) {
	
	// Create array of mode strings
    vector<string> modes = {orientation};
    //if (app->_defaultDisplay->sizeDiagonalInches()>=7) {
    //    modes.push_back("tablet");
    //} else {
        modes.push_back("phone");
    //}
    /*if (app->compactMode()) {
		modes.push_back("compact");
	}
    if (app->carouselsMode()) {
		modes.push_back("carousels");
	}*/
	if (site.length()) {
		modes.push_back(site);
	}
	/*todo if ([BBCNFeatureController tabBarEnabled]) {
		[modes addObject:@"tabs"];
	}
	if ([BBCNFeatureController advertsEnabled]) {
		[modes addObject:@"adverts"];
	}*/
	
    vector<string>& policyLayoutModes = BNPolicy::current()->_layoutModes;
	if (policyLayoutModes.size()) {
		modes.insert(modes.end(), policyLayoutModes.begin(), policyLayoutModes.end());
	}

	BNOptimizedLayout* optimizedLayout = chooseLayoutForModelId(modelId, site, format, modes);
	BNLayout* layout = optimizedLayout->layoutForModes(modes, _incBuffer);
	
	// Make a copy of the layout and remove any modules that are specific to a different content object
    layout = layout->clone();
	if (layout && layout->_rootContainer) {
		list<BNModule*> a;
		a.push_back(layout->_rootContainer);
		while (a.size()) {
            sp<BNModule> module = a.front();
            a.pop_front();
			string contentId = module->_json.stringVal("contentId");
			string contentLink = module->_json.stringVal("contentLink");
			if ((contentId.length() && contentId != modelId)
                || (contentLink.length() && contentLink == modelId)) {
                vector<sp<BNModule>> ma;
                for (auto& rm : module->_container->_modules) {
                    if (rm != module) {
                        ma.push_back(rm->clone());
                    }
                }
                module->_container->_modules  = ma;
			} else {
				if (module->isContainer()) {
                    auto& mods = module.as<BNContainerModule>()->_modules;
					a.insert(a.end(), mods.begin(), mods.end());
				}
			}
		}
    }

	return layout;
}

static bool contains(const vector<string>& vec, const string& val) {
    return std::find(vec.begin(), vec.end(), val) != vec.end();
}

BNOptimizedLayout* BNLayoutsGroup::chooseLayoutForModelId(const string& modelId,
                                                          const string& site,
                                                          const string& format,
                                                          const vector<string>& modes) {

	BNOptimizedLayout* bestMatch = NULL;
	int bestMatchCriteriaCount = 0;

	// Filter out mismatches
    for (BNOptimizedLayout* layout : _optimizedLayouts) {
		int matchCriteriaCount = 0;

		
		// Root-level modes check
		if (layout->_modes.size()) {
			bool modesOk = true;
            for (string& mode : layout->_modes) {
                if (!contains(modes, mode)) {
					modesOk = false;
				}
			}
			if (!modesOk) {
				continue;
			}
			//matchCriteriaCount++;
		}
		
		// Content/model ID is the highest-priority match, so if we find a match we can return it immediately
		if (layout->_content.size()) {
            if (contains(layout->_content, modelId)) {
				bestMatch = layout;
				break;
			}
			//continue;
		}
		
		// More general matching: site and formats
		if (site.length() && layout->_site.size()) {
			if (!contains(layout->_site, site)) {
				continue;
			}
			matchCriteriaCount++;
		}
		if (format.length() && layout->_format.size()) {
			if (!contains(layout->_format, format)) {
				continue;
			}
			matchCriteriaCount++;
		}
		
		
		if (matchCriteriaCount > bestMatchCriteriaCount) {
			bestMatchCriteriaCount = matchCriteriaCount;
			bestMatch = layout;
		}
	}
	
	// Use a default if no matching layout found
	if (!bestMatch) {
		log_warn("No layout found for %s. Using default index layout", modelId.c_str());
		bestMatch = _defaultOptimizedLayout;
	}
	

	return bestMatch;
}


