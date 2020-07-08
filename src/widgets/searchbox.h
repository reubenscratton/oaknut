//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class SearchBox  : public EditText {
public:
    
    // API
    SearchBox();
    typedef std::function<void(SearchBox*,const string&)> SEARCHTEXTCHANGED;
    virtual void setSearchTextChangedDelegate(SEARCHTEXTCHANGED delegate);
    
    // Overrides
    virtual void layout(RECT constraint) override;
    virtual void onStateChanged(VIEWSTATE changes) override;
    virtual void setPadding(EDGEINSETS padding) override;
    virtual void setText(const attributed_string& text) override;

protected:
    RECT getIconRect(float val);
    float spaceForSearchIcon();
    
    SEARCHTEXTCHANGED _searchTextChangedDelegate;
    sp<Task> _iconTask;
    TextureRenderOp* _searchIconOp;

};
