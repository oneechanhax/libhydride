/*
 * input.c
 *
 *  Created on: Nov 9, 2017
 *      Author: nullifiedcat
 */

#include "input.h"
#include "log.h"

#include <fcntl.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

int fd_keyboard = -1;
int fd_mouse = -1;

int *input_devices[] = {
        &fd_keyboard,
        &fd_mouse
};

int init_input()
{
    DIR *dev_input;
    struct dirent *ent;
    char buffer[400];
    dev_input = opendir("/dev/input/by-id");
    if (dev_input == NULL)
    {
        perror("opendir");
        return -1;
    }
    while ((ent = readdir(dev_input)) != NULL)
    {
        if (fd_keyboard < 0 && strstr(ent->d_name, "-event-kbd"))
        {
            snprintf(buffer, 400, "/dev/input/by-id/%s", ent->d_name);
            fd_keyboard = open(buffer, O_RDONLY);
            if (fd_keyboard < 0)
            {
                perror("Could not open keyboard");
                return -1;
            }
        }
        if (fd_mouse < 0 && strstr(ent->d_name, "-event-mouse"))
        {
            snprintf(buffer, 400, "/dev/input/by-id/%s", ent->d_name);
            fd_mouse = open(buffer, O_RDONLY);
            if (fd_mouse < 0)
            {
                perror("Could not open mouse");
                return -1;
            }
        }
    }

    int flags;
    flags = fcntl(fd_keyboard, F_GETFL, 0);
    fcntl(fd_keyboard, F_SETFL, flags | O_NONBLOCK);
    flags = fcntl(fd_mouse, F_GETFL, 0);
    fcntl(fd_mouse, F_SETFL, flags | O_NONBLOCK);
    return 0;
}

int poll_low_event(int fd, struct input_event *event)
{
    int reads = read(fd, event, sizeof(struct input_event));
    if (reads != sizeof(struct input_event))
    {
        return 0;
    }
    return 1;
}

int poll_event(int device, struct input_event_parsed *event)
{
    struct input_event ie;
    int failcount = 0;
    while (failcount++ < 2)
    {
        if (poll_low_event(*input_devices[device], &ie))
            break;
    }
    if (failcount >= 2)
        return 0;
    switch (ie.type)
    {
    case EV_KEY:
        event->type = INPUT_EVENT_KEY;
        event->evt_key.key = ie.code;
        event->evt_key.state = ie.value;
        break;
    case EV_REL:
        switch (ie.code)
        {
        case REL_X:
            event->type = INPUT_EVENT_MOVE;
            event->evt_move.x = ie.value;
            event->evt_move.y = 0;
            break;
        case REL_Y:
            event->type = INPUT_EVENT_MOVE;
            event->evt_move.x = 0;
            event->evt_move.y = ie.value;
            break;
        case REL_WHEEL:
            event->type = INPUT_EVENT_SCROLL;
            event->evt_scroll.value = ie.value;
            break;
        default:
            return 0;
        }
        break;
    default:
        return 0;
    }
    return 1;
}
