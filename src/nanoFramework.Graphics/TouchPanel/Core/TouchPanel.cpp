//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "TouchPanel.h"
#include "TouchDevice.h"
#include "TouchInterface.h"
#include <nanoPAL_Events.h>

#define abs(a)  (((a) < 0) ? -(a) : (a))
#define NOT_SET -1

#define TOUCH_SAMPLING_PERIOD_MILLISECONDS   10
#define TOUCH_PANEL_MINIMUM_GESTURE_DISTANCE 10
#define TOUCH_POINT_BUFFER_SIZE              200

TouchPanel g_TouchPanel;
extern TouchInterface g_TouchInterface;
extern TouchDevice g_TouchDevice;
extern DisplayDriver g_DisplayDriver;
extern GraphicsDriver g_GraphicsDriver;

TouchPoint g_PAL_TouchPointBuffer[TOUCH_POINT_BUFFER_SIZE];

#pragma region Touch
static CLR_UINT16 x_start_gesture = NOT_SET;
static CLR_UINT16 y_start_gesture = NOT_SET;
static CLR_UINT16 x_end_gesture = NOT_SET;
static CLR_UINT16 y_end_gesture = NOT_SET;
static CLR_UINT16 previousX = NOT_SET;
static CLR_UINT16 previousY = NOT_SET;
static CLR_UINT16 touch_move_count = 0;

HRESULT TouchPanel::Initialize()
{
    g_TouchPanel.m_InternalFlags = 0;
    g_TouchPanel.m_head = 0;
    g_TouchPanel.m_tail = 0;

    if (!g_TouchDevice.Enable(TouchIsrProc))
    {
        return CLR_E_FAIL;
    }
    g_TouchPanel.m_touchCompletion.InitializeForISR(TouchPanel::TouchCompletion, NULL);
    return S_OK;
}
HRESULT TouchPanel::Uninitialize()
{
    if (g_TouchPanel.m_touchCompletion.IsLinked())
    {
        g_TouchPanel.m_touchCompletion.Abort();
    }
    g_TouchDevice.Disable();
    return S_OK;
}
void TouchPanel::TouchCompletion(void *arg)
{
    (void)arg;
    g_TouchPanel.PollTouchPoint();
}
void TouchPanel::PollTouchPoint()
{
    CLR_INT32 x = 0;
    CLR_INT32 y = 0;
    TouchPoint *point = NULL;

    CLR_INT64 time = 0;
    TouchPointDevice devicePoint = g_TouchDevice.GetPoint();
    x = devicePoint.x;
    y = devicePoint.y;

    bool ContactDown = m_InternalFlags & Contact_Down;
    bool ContactWasDown = m_InternalFlags & Contact_WasDown;
    bool TOUCH_DOWN = (ContactDown && !ContactWasDown);
    bool TOUCH_MOVE = (ContactDown && ContactWasDown);
    bool TOUCH_UP = (!ContactDown && ContactWasDown);

    if (TOUCH_DOWN)
    {
        m_InternalFlags |= Contact_WasDown;

        point = AddTouchPoint(x, y, time);
        previousX = x;
        previousY = y;
        x_start_gesture = x;
        y_start_gesture = y;
        x_end_gesture = NOT_SET;
        y_end_gesture = NOT_SET;
        PostManagedEvent(EVENT_TOUCH, TouchPanelDown, 1, (CLR_UINT32)point);
    }
    else if (TOUCH_MOVE)
    {
        touch_move_count++;
        if ((abs(x - previousX) > 2 && abs(y - previousY) > 2))
        {
            point = AddTouchPoint(x, y, time);
            previousX = x;
            previousY = y;
            PostManagedEvent(EVENT_TOUCH, TouchPanelMove, 1, (CLR_UINT32)point);
        }
    }
    else if (TOUCH_UP)
    {
        m_InternalFlags &= ~Contact_WasDown;

        point = AddTouchPoint(x, y, time);
        previousX = x;
        previousY = y;
        x_end_gesture = x;
        y_end_gesture = y;

        // Only send a move event on touch up if we have more than two previous touch down events
        if (touch_move_count > 2 && (abs(x - previousX) > 2 && abs(y - previousY) > 2))
        {
            PostManagedEvent(EVENT_TOUCH, TouchPanelMove, 1, (CLR_UINT32)point);
            touch_move_count = 0;
        }
        PostManagedEvent(EVENT_TOUCH, TouchPanelUp, 1, (CLR_UINT32)point);
        m_InternalFlags &= ~Contact_WasDown;

        TouchGestures detectedGesture = GestureDetect(x_start_gesture, y_start_gesture);
        if (detectedGesture != TouchGestures::TouchGesture_NoGesture)
        {
            PostManagedEvent(EVENT_GESTURE, detectedGesture, 0, ((CLR_UINT32)x_start_gesture << 16) | y_start_gesture);
        }
    }

    // Schedule or unschedule completion.
    bool isLinked = g_TouchPanel.m_touchCompletion.IsLinked();
    if (!isLinked)
    {
        if (ContactDown)
        {
            m_touchCompletion.EnqueueDelta(TOUCH_SAMPLING_PERIOD_MILLISECONDS);
        }
    }
    else
    {
        if (!ContactDown)
        {
            g_TouchPanel.m_touchCompletion.Abort();
        }
    }
}

TouchPoint *TouchPanel::AddTouchPoint(CLR_UINT16 x, CLR_UINT16 y, CLR_INT64 time)
{

    CLR_UINT32 location = (x & 0x3FFF) | ((y & 0x3FFF) << 14);
    TouchPoint &point = g_PAL_TouchPointBuffer[m_tail];
    point.time = time;
    point.location = location;
    //point.contact = TouchPointContactFlags_Primary;

    GLOBAL_LOCK();
    {
        m_tail++;

        if (m_tail >= (CLR_INT32)TOUCH_POINT_BUFFER_SIZE)
            m_tail = 0;

        if (m_tail == m_head)
        {
            m_head++;

            if (m_head >= (CLR_INT32)TOUCH_POINT_BUFFER_SIZE)
                m_head = 0;
        }
    }
    GLOBAL_UNLOCK();

    return &point;
}
// When a falling or rising edge interrupt is detected
// (Usually, touch down, touch up)
// This routine is called to set the states and immediately queue
// a call back to the PollTouch Routine
void TouchPanel::TouchIsrProc(GPIO_PIN pin, bool touchStateDown, void *pArg)
{
    (void)pin;
    (void)pArg;

    GLOBAL_LOCK();
    {
        if (touchStateDown)
        {
            g_TouchPanel.m_InternalFlags |= Contact_Down;
        }
        else
        {
            g_TouchPanel.m_InternalFlags &= ~Contact_Down;
        }

        if (g_TouchPanel.m_touchCompletion.IsLinked())
        {
            g_TouchPanel.m_touchCompletion.Abort();
        }
        g_TouchPanel.m_touchCompletion.EnqueueDelta(0);
    }
    GLOBAL_UNLOCK();
}
#pragma endregion

#pragma region Gesture Detection

TouchGestures TouchPanel::GestureDetect(CLR_INT16 x, CLR_INT16 y)
{
    bool diagonal = false;
    CLR_INT16 dx = x_end_gesture - x;
    CLR_INT16 dy = y_end_gesture - y;
    CLR_INT16 adx = abs(dx);
    CLR_INT16 ady = abs(dy);
    TouchGestures dir = TouchGestures::TouchGesture_NoGesture;

    if ((adx + ady) >= TOUCH_PANEL_MINIMUM_GESTURE_DISTANCE)
    {
        // Diagonal line is defined as a line with angle between 22.5 degrees and 67.5
        // (and equivalent translations in each quadrant).
        // which means the
        // abs(dx) - abs(dy) <= abs(dx) if dx > dy or abs(dy) -abs(dx) <=  abs(dy) if dy > dx
        if (adx > ady)
        {
            diagonal = (adx - ady) <= (adx / 2);
        }
        else
        {
            diagonal = (ady - adx) <= (ady / 2);
        }
        if (diagonal)
        {
            if (dx > 0)
            {
                if (dy > 0)
                    dir = TouchGesture_DownRight; // SE.
                else
                    dir = TouchGesture_UpRight; // NE.
            }
            else
            {
                if (dy > 0)
                    dir = TouchGesture_DownLeft; // SW
                else
                    dir = TouchGesture_UpLeft; // NW.
            }
        }
        else if (adx > ady)
        {
            if (dx > 0)
                dir = TouchGesture_Right; // E.
            else
                dir = TouchGesture_Left; // W.
        }
        else
        {
            if (dy > 0)
                dir = TouchGesture_Down; // S.
            else
                dir = TouchGesture_Up; // N.
        }
        return dir;
    }
    return TouchGestures::TouchGesture_NoGesture;
}
#pragma endregion

