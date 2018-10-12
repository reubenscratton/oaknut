//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

VideoPlayer::VideoPlayer() : _status(Unready) {
}

void VideoPlayer::setStatus(Status newStatus) {
    if (newStatus == _status) {
        return;
    }
    _status = newStatus;
    if (_status == Ready && onReady) {
        onReady();
    }
}
void VideoPlayer::playToggle() {
    if (_status == Ready || _status == Paused || _status == Finished) {
        if (_status == Finished) {
            setCurrent(0);
        }
        play();
    } else if (_status == Playing) {
        pause();
    }
}
