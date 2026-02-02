/***************************************************************************
 *   Copyright (c) 2018 Torsten Sadowski <tsadowski[at]gmx.net>            *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/

#include <FCConfig.h>

#include "GuiNativeEventLinux.h"

#include "GuiApplicationNativeEventAware.h"
#include <Base/Console.h>
#include <QMainWindow>
#include <QLibrary>
#include <QSocketNotifier>

// Definitions from spnav.h

// NOLINTBEGIN
enum {
    SPNAV_EVENT_ANY,        /* used by spnav_remove_events() */
    SPNAV_EVENT_MOTION,
    SPNAV_EVENT_BUTTON,     /* includes both press and release */

    SPNAV_EVENT_DEV,        /* add/remove device event */
    SPNAV_EVENT_CFG,        /* configuration change event */

    SPNAV_EVENT_RAWAXIS,
    SPNAV_EVENT_RAWBUTTON
};

enum { SPNAV_DEV_ADD, SPNAV_DEV_RM };

struct spnav_event_motion {
    int type;               /* SPNAV_EVENT_MOTION */
    int x, y, z;
    int rx, ry, rz;
    unsigned int period;
    int *data;
};

struct spnav_event_button {
    int type;               /* SPNAV_EVENT_BUTTON or SPNAV_EVENT_RAWBUTTON */
    int press;
    int bnum;
};

struct spnav_event_dev {
    int type;               /* SPNAV_EVENT_DEV */
    int op;                 /* SPNAV_DEV_ADD / SPNAV_DEV_RM */
    int id;
    int devtype;            /* see spnav_dev_type() */
    int usbid[2];           /* USB id if it's a USB device, 0:0 if it's a serial device */
};

struct spnav_event_cfg {
    int type;               /* SPNAV_EVENT_CFG */
    int cfg;                /* same as protocol REQ_GCFG* enum */
    int data[6];            /* same as protocol response data 0-5 */
};

struct spnav_event_axis {
    int type;               /* SPNAV_EVENT_RAWAXIS */
    int idx;                /* axis number */
    int value;              /* value */
};

typedef union spnav_event {
    int type;
    struct spnav_event_motion motion;
    struct spnav_event_button button;
    struct spnav_event_dev dev;
    struct spnav_event_cfg cfg;
    struct spnav_event_axis axis;
} spnav_event;
// NOLINTEND


// NOLINTBEGIN
typedef int(*dl_spnav_open)();
typedef int(*dl_spnav_close)();
typedef int(*dl_spnav_fd)();
typedef int(*dl_spnav_poll_event)(spnav_event *);
typedef int(*dl_spnav_remove_events)(int);
typedef int(*dl_spnav_dev_name)(char*, int);
typedef int(*dl_spnav_client_name)(const char *);
static QString spnavLib(QLatin1String("spnav"));
constexpr int versionNumber = 0;
// NOLINTEND

Gui::GuiNativeEvent::GuiNativeEvent(Gui::GUIApplicationNativeEventAware *app)
: GuiAbstractNativeEvent(app)
{
}

Gui::GuiNativeEvent::~GuiNativeEvent()
{
    dl_spnav_close spnav_close = (dl_spnav_close)QLibrary::resolve(spnavLib, versionNumber, "spnav_close");
    if (!spnav_close || connectDaemon == -1) {
        return;
    }
    if (spnav_close()) {
        Base::Console().Log("Couldn't disconnect from spacenav daemon\n");
    }
    else {
        Base::Console().Log("Disconnected from spacenav daemon\n");
    }
}

void Gui::GuiNativeEvent::initSpaceball(QMainWindow *window)
{
    Q_UNUSED(window)
    dl_spnav_open spnav_open = (dl_spnav_open)QLibrary::resolve(spnavLib, versionNumber, "spnav_open");
    dl_spnav_fd spnav_fd = (dl_spnav_fd)QLibrary::resolve(spnavLib, versionNumber, "spnav_fd");
    dl_spnav_dev_name spnav_dev_name = (dl_spnav_dev_name)QLibrary::resolve(spnavLib, versionNumber, "spnav_dev_name");
    dl_spnav_client_name spnav_client_name = (dl_spnav_client_name)QLibrary::resolve(spnavLib, versionNumber, "spnav_client_name");
    if (!spnav_open || !spnav_fd) {
        return;
    }

    connectDaemon = spnav_open();
    if (connectDaemon == -1) {
        Base::Console().Log("Couldn't connect to spacenav daemon. Please ignore if you don't have a spacemouse.\n");
    }
    else {
        Base::Console().Log("Connected to spacenav daemon\n");
        QSocketNotifier* spacenavNotifier = new QSocketNotifier(spnav_fd(), QSocketNotifier::Read, this);
        connect(spacenavNotifier, &QSocketNotifier::activated, this, &GuiNativeEvent::pollSpacenav);
        mainApp->setSpaceballPresent(true);

        if (spnav_client_name) {
            spnav_client_name("FreeCAD");
        }

        if (spnav_dev_name) {
            std::vector<char> buffer(100);
            spnav_dev_name(buffer.data(), static_cast<int>(buffer.size()));
            mainApp->setDeviceName(QString::fromLatin1(buffer.data()));
        }
    }
}

void Gui::GuiNativeEvent::pollSpacenav()
{
    dl_spnav_poll_event spnav_poll_event = (dl_spnav_poll_event)QLibrary::resolve(spnavLib, versionNumber, "spnav_poll_event");
    dl_spnav_remove_events spnav_remove_events = (dl_spnav_remove_events)QLibrary::resolve(spnavLib, versionNumber, "spnav_remove_events");
    if (!spnav_poll_event) {
        return;
    }

    spnav_event ev;
    while(spnav_poll_event(&ev))
    {
        switch (ev.type)
        {
            case SPNAV_EVENT_MOTION:
            {
                motionDataArray[0] = -ev.motion.x;
                motionDataArray[1] = -ev.motion.z;
                motionDataArray[2] = -ev.motion.y;
                motionDataArray[3] = -ev.motion.rx;
                motionDataArray[4] = -ev.motion.rz;
                motionDataArray[5] = -ev.motion.ry;
                if (spnav_remove_events) {
                    spnav_remove_events(SPNAV_EVENT_MOTION);
                }
                mainApp->postMotionEvent(motionDataArray);
                break;
            }
            case SPNAV_EVENT_BUTTON:
            {
                mainApp->postButtonEvent(ev.button.bnum, ev.button.press);
                break;
            }
            case SPNAV_EVENT_DEV:
            {
                // op=0: add device, op=1: remove device
                const int op = ev.dev.op;
                mainApp->setSpaceballPresent(op == 0);

                dl_spnav_dev_name spnav_dev_name = (dl_spnav_dev_name)QLibrary::resolve(spnavLib, versionNumber, "spnav_dev_name");
                if (spnav_dev_name) {
                    std::vector<char> buffer(100);
                    spnav_dev_name(buffer.data(), static_cast<int>(buffer.size()));
                    mainApp->setDeviceName(QString::fromLatin1(buffer.data()));
                }
            }
        }
    }
}

#include "3Dconnexion/moc_GuiNativeEventLinux.cpp"
