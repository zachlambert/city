#pragma once


struct KeyState {
    bool down;
    mutable bool just_changed;
    bool pressed()const
    {
        if (down && just_changed) {
            just_changed = false;
            return true;
        }
        return false;
    };
    bool released()const
    {
        if (!down && just_changed) {
            just_changed = false;
            return true;
        }
        return false;
    };

    KeyState():
        down(false),
        just_changed(false)
    {}
};

struct MouseMode {
    bool cursor_hidden;
    bool raw_mouse_motion;
    MouseMode():
        cursor_hidden(false),
        raw_mouse_motion(false)
    {}
};
