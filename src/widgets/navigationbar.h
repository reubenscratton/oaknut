//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class NavigationBar : public View {
public:

	//ObjPtr<NavigationItem> _navigationItem;
	bool _blurEnabled;
    COLOR _backgroundColor;
    
	NavigationBar();
	
	virtual void setBackground(RenderOp* renderOp);
    virtual void setBackgroundColor(COLOR backgroundColor);
    virtual void addNavigationItem(NavigationItem* navigationItem);
    virtual void removeNavigationItem(NavigationItem* navigationItem);
    virtual void setBlurEnabled(bool blurEnabled);

};


