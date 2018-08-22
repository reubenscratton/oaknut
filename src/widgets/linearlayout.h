//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class LinearLayout : public View {
public:

    LinearLayout();
    
    // API
    enum {
        Horizontal,
        Vertical
    } _orientation;
    
	// Overrides
    void measure(float parentWidth, float parentHeight) override;
	void layout() override;
	void addSubview(View* subview) override;
	void removeSubview(View* subview) override;
    bool applyStyleValue(const string& name, const StyleValue* value) override;
    bool applyStyleValueFromChild(const string& name, const StyleValue* value, View* subview) override;

protected:
    void setWeight(View* subview, float weight);

    vector<float> _weights;
    float _weightsTotal;

};
