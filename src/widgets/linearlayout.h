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
    void setSubviewWeight(View* subview, float weight);

	// Overrides
    void layout(RECT constraint) override;
    void layoutSubviews(RECT constraint) override;
    void insertSubview(View* subview, int index) override;
	void removeSubview(View* subview) override;
    bool applySingleStyle(const string& name, const style& value) override;

protected:

    vector<float> _weights;
    float _weightsTotal;
    float _spacing;
    bool _updateSubviewAligns;

};
