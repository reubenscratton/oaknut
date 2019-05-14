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
    void setSpacing(float spacing);
    enum {
        Horizontal,
        Vertical
    } _orientation;
    
	// Overrides
    void layoutSubviews(RECT constraint) override;
	void addSubview(View* subview) override;
	void removeSubview(View* subview) override;
    bool applySingleStyle(const string& name, const style& value) override;

protected:
    void setWeight(View* subview, float weight);

    vector<float> _weights;
    float _weightsTotal;
    float _spacing;

};
