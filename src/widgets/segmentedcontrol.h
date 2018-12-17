//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


/**
@ingroup widgets
@brief iOS style button group
@verbatim

 #### Styles
 
 Name         | Type                   | Remarks
 ------------ | ---------------------- | -------
 `corner-radius` | measurement |
 `font-name`  | text        |
 `font-size`  | measurement  |
 `font-weight`| fontweight   |
 `stroke-width` | measurement  |

@endverbatim
*/
class SegmentedControl : public View {
public:
	
    /**  @cond INTERNAL */
    SegmentedControl();
    /**  @endcond */
    
    /** @name Methods
      * @{ */
    virtual void setFontName(const string& fontName);
    virtual void setFontSize(float fontSize);
    virtual void setFontWeight(float fontWeight);
	virtual void addSegment(const string& label);
    virtual void setTextColor(COLOR color);
	virtual void setSelectedTextColor(COLOR color);
	virtual void onSegmentTap(int segmentIndex);
	virtual void setSelectedIndex(int segmentIndex);
    virtual int getSelectedIndex() const { return _selectedIndex; }
    virtual void setPressedIndex(int pressedIndex);
    /** @} */
    
    /** @name Events
      * @{ */
    std::function<void(int)> onSegmentSelected;
    /** @} */

    /** @name Overrides
     * @{ */
    bool applyStyleValue(const string &name, const StyleValue *value) override;
	void updateContentSize(SIZE constrainingSize) override;
	void layout(RECT constraint) override;
    void updateRenderOps() override;
	bool handleInputEvent(INPUTEVENT* event) override;
    void onEffectiveTintColorChanged() override;
    /** @} */

protected:
    sp<Font> _font;
    string _fontName;
    float _fontSize;
    float _fontWeight;
    bool _fontValid;
    typedef struct {
        RECT rect;
        sp<RectRenderOp> rectOp;
        sp<TextRenderer> label;
    } Segment;
    vector<Segment> _segments;
    COLOR _textColor;
    COLOR _selectedTextColor;
    float _lineWidth;
    float _cornerRadius;
    int _pressedIndex;
    int _selectedIndex;

    void updateBorders();
};
