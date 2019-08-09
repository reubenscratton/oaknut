//
//  emBeeb
//
//  Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//

#pragma once
#include "../app.h"
#include "../model/diskinfo.h"


class GameItem : public Object {
public:
	Game* _game;
	DiskInfo* _selectedDiskInfo;
	
	GameItem(Game* game);
	string getTitle() const;
	string getSubtitle() const;
	string getImageUrl() const;
};

class GameItemView : public View {
public:
    GameItemView() {
    }
    GameItemView(bool best) {
        app->layoutInflateExistingView(this, best ? "layout/disk_listitem_best.res" : "layout/disk_listitem.res");
        numberLabel = (Label*)findViewById("number");
        imageView = (ImageView*)findViewById("image");
        titleLabel = (Label*)findViewById("title");
        subtitleLabel = (Label*)findViewById("subtitle");
    }
    void bind(GameItem& gameItem, int index) {
        if (numberLabel) {
            numberLabel->setText(string::format("%d.", index));
        }
        imageView->setImageUrl(gameItem.getImageUrl());
        titleLabel->setText(gameItem.getTitle());
        subtitleLabel->setText(gameItem.getSubtitle());
    }
    Label* numberLabel;
    ImageView* imageView;
    Label* titleLabel;
    Label* subtitleLabel;
};

