#include "av.h"


class BNAudio : public BNAV {
public:
    BNAudio(const variant& json);

    bool isAudio() override;
};
