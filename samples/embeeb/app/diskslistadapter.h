//
//  emBeeb
//
//  Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//

#pragma once
#include "app.h"
#include "diskinfo.h"


class DiskItem : public Object {
public:
	Game* _game;
	DiskInfo* _selectedDiskInfo;
	
	DiskItem(Game* game);
	string getTitle();
	string getSubtitle();
	string getImageUrl();
};

class DiskItemView : public View {
public:
    DiskItemView() {
        app.layoutInflateExistingView(this, "layout/disk_listitem.res");
        numberLabel = (Label*)findViewById("number");
        imageView = (ImageView*)findViewById("image");
        titleLabel = (Label*)findViewById("title");
        subtitleLabel = (Label*)findViewById("subtitle");
    }
    void bind(DiskItem& diskItem, int index) {
        if (numberLabel) {
            numberLabel->setText(string::format("%d.", index));
        }
        imageView->setImageUrl(diskItem.getImageUrl());
        titleLabel->setText(diskItem.getTitle());
        subtitleLabel->setText(diskItem.getSubtitle());
    }
    Label* numberLabel;
    ImageView* imageView;
    Label* titleLabel;
    Label* subtitleLabel;
};

class DisksListAdapter : public SimpleListAdapter<DiskItem, DiskItemView> {
public:
	DisksListAdapter(string srcfile);
    ~DisksListAdapter();
	
protected:
    string srcfile;
    
    void bindItemView(View* itemview, LISTINDEX index) override;
    virtual void handleJson(const variant& json);
};

