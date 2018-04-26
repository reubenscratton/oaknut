//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class NavigationBar : public View {
public:

	//ObjPtr<NavigationItem> _navigationItem;
	bool _blurEnabled;
    COLOUR _backgroundColour;
    
	NavigationBar();
	
	virtual void setBackground(RenderOp* renderOp);
    virtual void setBackgroundColour(COLOUR backgroundColour);
    virtual void addNavigationItem(NavigationItem* navigationItem);
    virtual void removeNavigationItem(NavigationItem* navigationItem);
    virtual void setBlurEnabled(bool blurEnabled);

};


