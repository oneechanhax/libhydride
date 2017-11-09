/*
 * input.h
 *
 *  Created on: Nov 9, 2017
 *      Author: nullifiedcat
 */

#pragma once

#include <linux/input.h>

enum
{
    INPUT_DEVICE_KEYBOARD,
    INPUT_DEVICE_MOUSE
};

enum
{
    INPUT_EVENT_KEY,
    INPUT_EVENT_MOVE,
    INPUT_EVENT_SCROLL
};

struct input_event_parsed
{
    int type;
    union
    {
        struct
        {
            int key;
            int state;
        } evt_key;
        struct
        {
            int x;
            int y;
        } evt_move;
        struct
        {
            int value;
        } evt_scroll;
    };
};

int init_input();
int poll_low_event(int fd, struct input_event *event);
int poll_event(int device, struct input_event_parsed *event);
