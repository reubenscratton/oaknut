#pragma once
#include "basemodel.h"

class BNImage : public BNBaseModel {
public:
    string _altText;
    string _caption;
    string _copyrightHolder;
    int _height;
    int _width;
    string _positionHint;


    BNImage(const variant& json);
    bool isProbablyAHorizontalRule();
    virtual string urlForWidth(float width);

    static bool isInlineImage(SIZE imageSize, SIZE viewSize);
    static SIZE sizeForImageInView(BNImage* image, View* view);

    bool isImage() override { return true; }
};



class BNStaticImage : public BNImage {
public:
    string urlForWidth(float width) override;

};
