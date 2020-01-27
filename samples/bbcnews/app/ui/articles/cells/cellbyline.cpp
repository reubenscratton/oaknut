//
//  Copyright (c) 2019 BBC News. All rights reserved.
//


#include "../../indexes/cells/cellitemtopstory.h"

//#import "BNPerson.h"
//#import "BNByline.h"


class BNCellByline : public BNCell {
public:
    
    BNCellByline(BNCellsModule* module) : BNCell(module) {
        setPadding(EDGEINSETS(32,16,32,16));
        _thumbnail = new BNImageView();
        _thumbnail->setLayoutSize(MEASURESPEC::Abs(app->dp(50)), MEASURESPEC::Abs(app->dp(50)));
        _thumbnail->setBackgroundColor(0xFFCCCCCC);
        // TODO: _thumbnail.layer.masksToBounds = YES;
        // TODO: _thumbnail.layer.cornerRadius = bylineThumbnailSize/2;
        addSubview(_thumbnail);
        _name = new Label();
        _name->setLayoutSize(MEASURESPEC::Wrap(), MEASURESPEC::Wrap());
        _name->setLayoutOrigin(ALIGNSPEC::ToRightOf(_thumbnail, app->dp(8)), ALIGNSPEC::Top());
        _name->setMaxLines(1);
        addSubview(_name);
        _function = new Label();
        _function->setLayoutSize(MEASURESPEC::Wrap(), MEASURESPEC::Wrap());
        _function->setLayoutOrigin(ALIGNSPEC::ToRightOf(_thumbnail, app->dp(8)), ALIGNSPEC::Below(_name, app->dp(8)));
        addSubview(_function);
    }
    
    void setRelationship(BNRelationship* rel) override {
        
        // NB: Bylines have horrific logic. The relationship provided to this function is not
        // necessarily the one we need. The first thing have have to do is fish all BNByline
        // objects and BNPerson objects out of the containing item.
        auto bylines = rel->_parentObject->findChildren({BNModelTypeByline}, {BNModelTypeByline}, {});
        auto people = rel->_parentObject->findChildren({BNModelTypePerson}, {BNModelTypeByline}, {});
        
        BNPerson *person = nullptr;

        if (bylines.size() > 0 ) {
            if (people.size() == 1) {
                person = (BNPerson*)people[0];
            } else {
                person = (BNPerson*)bylines[0];
            }
        }
        
        string name;
        if (people.size() > 0) {
            if (people.size() == 1){
                person = (BNPerson*)people[0];
                name = person->_name;
            } else {
                for(int i =0; i< people.size(); i++){
                    if (i != 0) {
                        name += ", ";
                    }
                    BNPerson *p = (BNPerson*)people[i];
                    if (p->_name.length()) {
                        name += p->_name;
                    }
                }
            }
        }
        
        _name->setText(name);
        
        string bylineText;
        if (person) {
            bylineText = person->_function;
            if (person->_thumbnailUrl){
                variant dict;
                dict.set("width", 50);
                dict.set("height", 50);
                BNImage* thumbImage = new BNImage(dict);
                string path = url(person->_thumbnailUrl).path;
                thumbImage->_modelId = "/mcs" + path;
                _thumbnail->setBNImage(thumbImage);
            }
        }
        
        // Allow the 'byline' object title, if present, to override the person's 'function'.
        if (bylines.size()) {
            BNByline* byline = (BNByline*)bylines[0];
            bylineText = byline->_title;
        }
        
        _function->setText(bylineText);
        //= [[BNLabelInfo alloc] initWithString:bylineText attributes:attrsBylineFunction numLines:0];

    }

    Label* _name;
    Label* _function;
    BNImageView* _thumbnail;

};

DECLARE_DYNCREATE(BNCellByline, BNCellsModule*);


