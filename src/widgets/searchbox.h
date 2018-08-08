//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
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
    virtual void layout();
    virtual bool setFocused(bool focused);
    virtual void setPadding(EDGEINSETS padding);
    virtual void setText(string text);

protected:
    float spaceForSearchIcon();
    
    SEARCHTEXTCHANGED _searchTextChangedDelegate;
    TextureRenderOp* _searchIconOp;

};
