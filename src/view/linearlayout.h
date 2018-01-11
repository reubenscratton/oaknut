//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class LinearLayout : public View {
public:
    enum {
		Horizontal,
		Vertical
	} _orientation;
	vector<float> _weights;
	float _weightsTotal;

    LinearLayout();
    //bool processAttribute(PCSTRING attrName, PCSTRING attrValue);
	
	// Overrides
    virtual void measure(float parentWidth, float parentHeight);
	//virtual void updateContentSize(float parentWidth, float parentHeight);
    virtual void layout();
	virtual void addSubview(View* subview);
	virtual void removeSubview(View* subview);
	virtual void setWeight(View* subview, float weight);
    virtual bool applyStyleValue(const string& name, StyleValue* value);
    virtual bool applyStyleValueFromChild(const string& name, StyleValue* value, View* subview);

};
