//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


/**
 * @ingroup app_group
 * @brief Contains the UI elements displayed in the nav bar in a NavigationController.
 */
class NavigationItem : public Object {
public:
    string _title;
	ObjPtr<class View> _titleView;
    ObjPtr<class LinearLayout> _leftButtonsFrame;
    ObjPtr<LinearLayout> _rightButtonsFrame;
	
    NavigationItem();
	static class ImageView* createIconButton(const string& src, std::function<void()> onClick);
	void setTitle(const string& title);
	void setTitleView(View* titleView);
	void addLeftButton(ImageView* button);
	void addRightButton(ImageView* button);
	void applyTransition(class NavigationBar* navBar, float val, bool incoming, bool isPop);
};

