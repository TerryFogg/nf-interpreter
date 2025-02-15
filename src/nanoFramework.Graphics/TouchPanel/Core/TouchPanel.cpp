//
//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "TouchPanel.h"
#include "TouchDevice.h"
#include "TouchInterface.h"
#include <nanoPAL_Events.h>

#define ONE_MHZ 1000000
#define abs(a)  (((a) < 0) ? -(a) : (a))
#define sign(a) (((a) < 0) ? -1 : 1)

#define TOUCH_POINT_BUFFER_SIZE 200

TouchPanel g_TouchPanel;
extern TouchInterface g_TouchInterface;
extern TouchDevice g_TouchDevice;
extern DisplayDriver g_DisplayDriver;
extern GraphicsDriver g_GraphicsDriver;

TOUCH_PANEL_SamplingSettings g_TouchPanel_Sampling_Settings = {
    // ActivePinStateForTouchDown
    FALSE,
    // TOUCH_PANEL_SAMPLE_RATE
    // Minimum Sampling Rate
    {50,
     // longer Sampling Rate
     200,
     // Current Sampling Rate
     10,
     // Maximum Time For Move Event in milliseconds
     50}};
TOUCH_PANEL_CalibrationData g_TouchPanel_Calibration_Config = {1, 1, 0, 0, 1, 1};
TOUCH_PANEL_CalibrationData g_TouchPanel_DefaultCalibration_Config = {1, 1, 0, 0, 1, 1};
TouchPoint g_PAL_TouchPointBuffer[TOUCH_POINT_BUFFER_SIZE];
InkRegionInfo m_InkRegionInfo;

CLR_UINT16 m_x_start_gesture = 0;
CLR_UINT16 m_y_start_gesture = 0;

CLR_UINT16 m_lastx_gesture = 0;
CLR_UINT16 m_lasty_gesture = 0;

// Divide a by b, and return rounded integer.
CLR_INT32 RoundDiv(CLR_INT32 a, CLR_INT32 b)
{
    if (b == 0)
        return 0xFFFF;

    CLR_INT32 d = a / b;
    CLR_INT32 r = abs(a) % abs(b);

    if ((r * 2) > abs(a))
    {
        d = (abs(d) + 1) * sign(b) * sign(a);
    }

    return d;
}

#pragma region Touch

HRESULT TouchPanel::Initialize()
{
    g_TouchPanel.m_samplingPeriod = g_TouchPanel_Sampling_Settings.SampleRate.SamplingPeriodCurrent;

    g_TouchPanel.m_calibrationData = g_TouchPanel_Calibration_Config;
    g_TouchPanel.m_InternalFlags = 0;
    g_TouchPanel.m_head = 0;
    g_TouchPanel.m_tail = 0;
    g_TouchPanel.m_startMovePtr = NULL;
    g_TouchPanel.m_touchMoveIndex = 0;

    // Enable the touch hardware.
    if (!g_TouchDevice.Enable(TouchIsrProc))
    {
        return CLR_E_FAIL;
    }
    g_TouchPanel.m_touchCompletion.InitializeForISR(TouchPanel::TouchCompletion, NULL);
    // At this point we should be ready to recieve touch inputs.

    m_InkRegionInfo.Bmp == NULL;

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

// Calibrate an uncalibrated point.
void TouchPanel::TouchPanelCalibratePoint(CLR_INT32 UncalX, CLR_INT32 UncalY, CLR_INT32 *pCalX, CLR_INT32 *pCalY)
{
    // Doing simple linear calibration for now.
    // ASSUMPTION: uncalibrated x, y touch co-ordinates are independent. In reality this is not correct.
    // We should consider doing 2D calibration that takes dependeng_Cy issue into account.

    *pCalX = RoundDiv((m_calibrationData.Mx * UncalX + m_calibrationData.Cx), m_calibrationData.Dx);
    *pCalY = RoundDiv((m_calibrationData.My * UncalY + m_calibrationData.Cy), m_calibrationData.Dy);

    // FUTURE - 07/10/2008-munirula- Negative co-ords are meaningful or meaningless. I am leaning
    // towards meaningless for now, but this needs further thought which I agree.
    if (*pCalX < 0)
        *pCalX = 0;

    if (*pCalY < 0)
        *pCalY = 0;
}

HRESULT TouchPanel::GetDeviceCaps(unsigned int iIndex, void *lpOutput)
{
    if (lpOutput == NULL)
    {
        return FALSE;
    }

    switch (iIndex)
    {
        case TOUCH_PANEL_SAMPLE_RATE_ID:
        {
            TOUCH_PANEL_SAMPLE_RATE *pTSR = (TOUCH_PANEL_SAMPLE_RATE *)lpOutput;

            pTSR->SamplingPeriodLow = g_TouchPanel_Sampling_Settings.SampleRate.SamplingPeriodLow;
            pTSR->SamplingPeriodHigh = g_TouchPanel.SampleRate.SamplingPeriodHigh;
            pTSR->SamplingPeriodCurrent = g_TouchPanel.SampleRate.SamplingPeriodCurrent;
            pTSR->MaxTimeForMoveEvent_Milliseconds = g_TouchPanel.SampleRate.MaxTimeForMoveEvent_Milliseconds;
        }
        break;

        case TOUCH_PANEL_CALIBRATION_POINT_COUNT_ID:
        {
            TOUCH_PANEL_CALIBRATION_POINT_COUNT *pTCPC = (TOUCH_PANEL_CALIBRATION_POINT_COUNT *)lpOutput;

            pTCPC->flags = 0;
            pTCPC->cCalibrationPoints = 5;
        }
        break;

        case TOUCH_PANEL_CALIBRATION_POINT_ID:
            return (g_TouchPanel.CalibrationPointGet((TOUCH_PANEL_CALIBRATION_POINT *)lpOutput));

        default:
            return FALSE;
    }

    return TRUE;
}

HRESULT TouchPanel::ResetCalibration()
{
    g_TouchPanel.m_calibrationData = g_TouchPanel_DefaultCalibration_Config;

    return S_OK;
}

HRESULT TouchPanel::SetCalibration(int pointCount, CLR_INT16 *sx, CLR_INT16 *sy, CLR_INT16 *ux, CLR_INT16 *uy)
{
    if (pointCount != 5)
        return CLR_E_FAIL;

    // Calculate simple 1D calibration parameters.
    g_TouchPanel.m_calibrationData.Mx = sx[2] - sx[3];
    g_TouchPanel.m_calibrationData.Cx = ux[2] * sx[3] - ux[3] * sx[2];
    g_TouchPanel.m_calibrationData.Dx = ux[2] - ux[3];

    g_TouchPanel.m_calibrationData.My = sy[1] - sy[2];
    g_TouchPanel.m_calibrationData.Cy = uy[1] * sy[2] - uy[2] * sy[1];
    g_TouchPanel.m_calibrationData.Dy = uy[1] - uy[2];

    return S_OK;
}

void TouchPanel::TouchCompletion(void *arg)
{
    (void)arg;
    g_TouchPanel.PollTouchPoint();
}

static int touchdown1 = 0;
static int touchdown2 = 0;

void TouchPanel::PollTouchPoint()
{
    TOUCH_PANEL_SAMPLE_FLAGS flags;
    CLR_INT32 x = 0;
    CLR_INT32 y = 0;
    CLR_INT32 ux = 0; // Uncalibrated x.
    CLR_INT32 uy = 0; // Uncalibrated y.
    bool fProcessUp = false;

    CLR_INT64 time = ::HAL_Time_CurrentTime();

    // Get the point information from driver.
    GetPoint(&flags, &ux, &uy);

    // Driver should be doing all filter/averaging of the points.
    TouchPanelCalibratePoint(ux, uy, &x, &y);

    TouchPoint *point = NULL;
    volatile bool ContactDown = m_InternalFlags & Contact_Down;
    volatile bool ContactWasDown = m_InternalFlags & Contact_WasDown;
    volatile bool firstContact = ContactDown && !ContactWasDown;

    if (ContactDown && !ContactWasDown)
    {
        // Add contract down marker to touch points
        AddTouchPoint(TouchPointLocationFlags_ContactDown, TouchPointLocationFlags_ContactDown, time);
        // Add the touch point data to an array and post an up event
        if ((point = AddTouchPoint(x, y, time)) != NULL)
        {
            PostManagedEvent(EVENT_TOUCH, TouchPanelStylusDown, 1, (CLR_UINT32)point);
        }
        m_InternalFlags |= Contact_WasDown;
        m_x_start_gesture = x;
        m_y_start_gesture = y;
        m_lastx_gesture = 0;
        m_lasty_gesture = 0;
    }
    else if (!ContactDown && ContactWasDown)
    {
        fProcessUp = true;

        // Add the touch point data
        point = AddTouchPoint(x, y, time);
    }
    else if (ContactDown && ContactWasDown)
    {
        touchdown1++;
        point = AddTouchPoint(x, y, time);
        // Add point if not a duplicate
        if (point != NULL)
        {
            touchdown2++;

            PostManagedEvent(EVENT_TOUCH, TouchPanelStylusMove, m_touchMoveIndex, (CLR_UINT32)m_startMovePtr);
            m_startMovePtr = NULL;
            m_touchMoveIndex = 0;
        }
        // TouchDown state but hasn't moved, Update to the latest point
        else if (m_startMovePtr == NULL)
        {
            CLR_UINT32 touchflags = GetTouchPointFlags_LatestPoint;
            if (GetTouchPoint(&touchflags, &point) == S_OK)
            {
                m_startMovePtr = point;
                m_touchMoveIndex = 1;
            }
        }
    }

    if (!ContactDown && ContactWasDown)
    {
        fProcessUp = true;
    }

    if (fProcessUp)
    {
        CLR_UINT32 touchflags = GetTouchPointFlags_LatestPoint;
        TouchPoint *point = NULL;
        // Only send a move event if we have substantial data (more than two items) otherwise, the TouchUp event should
        // suffice
        if (m_touchMoveIndex > 2 && m_startMovePtr != NULL)
        {
            PostManagedEvent(EVENT_TOUCH, TouchPanelStylusMove, m_touchMoveIndex, (CLR_UINT32)m_startMovePtr);
        }
        m_startMovePtr = NULL;
        m_touchMoveIndex = 0;

        if (FAILED(GetTouchPoint(&touchflags, &point)))
        {
            point = NULL;
        }

        // Add contact up marker to touch points
        AddTouchPoint(TouchPointLocationFlags_ContactUp, TouchPointLocationFlags_ContactUp, time);
        PostManagedEvent(EVENT_TOUCH, TouchPanelStylusUp, 1, (CLR_UINT32)point);
        m_InternalFlags &= ~Contact_WasDown;

        GestureDetect(x, y);
    }

    // Schedule or unschedule completion.
    volatile bool isLinked = g_TouchPanel.m_touchCompletion.IsLinked();
    if (!isLinked)
    {
        if (ContactDown)
        {
            m_touchCompletion.EnqueueDelta(g_TouchPanel.m_samplingPeriod);
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
    static CLR_UINT16 lastAddedX = 0xFFFF;
    static CLR_UINT16 lastAddedY = 0xFFFF;

    if ((x == TouchPointLocationFlags_ContactUp) || (x == TouchPointLocationFlags_ContactDown))
    {
        // Reset the points.
        lastAddedX = 0xFFFF;
        lastAddedY = 0xFFFF;
    }
    else
    {
        // Touch Move
        if (lastAddedX != 0xFFFF)
        {
            // Check that we have moved at least 2 units
            CLR_INT32 dx = abs(x - lastAddedX);
            CLR_INT32 dy = abs(y - lastAddedY);
            if ((dx <= 2 && dy <= 2))
            {
                return NULL;
            }
        }
        lastAddedX = x;
        lastAddedY = y;
    }

    CLR_UINT32 location = (x & 0x3FFF) | ((y & 0x3FFF) << 14);
    CLR_UINT32 contact = TouchPointContactFlags_Primary | TouchPointContactFlags_Pen;
    TouchPoint &point = g_PAL_TouchPointBuffer[m_tail];
    point.time = time;
    point.location = location;
    point.contact = contact;

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

HRESULT TouchPanel::GetTouchPoints(int *pointCount, CLR_INT16 *sx, CLR_INT16 *sy)
{
    if (pointCount == NULL)
    {
    }; // Avoid unused parameter, maybe used in the future?
    if (sx == NULL)
    {
    }; // Avoid unused parameter, maybe used in the future?
    if (sy == NULL)
    {
    }; // Avoid unused parameter, maybe used in the future?

    // To be revisited::    GLOBAL_LOCK(isr);
    return S_OK;
}

HRESULT TouchPanel::GetSetTouchInfo(CLR_UINT32 flags, CLR_INT32 *param1, CLR_INT32 *param2, CLR_INT32 *param3)
{
    if (flags & TouchInfo_Set) // SET.
    {
        if (flags & TouchInfo_SamplingDistance)
        {
            CLR_INT32 samplesPerSecond = ONE_MHZ / *param1;

            // Minimum of 500us otherwise the system will be overrun
            if (samplesPerSecond >= g_TouchPanel_Sampling_Settings.SampleRate.SamplingPeriodLow &&
                samplesPerSecond <= g_TouchPanel_Sampling_Settings.SampleRate.SamplingPeriodHigh)
            {
                g_TouchPanel.m_samplingPeriod = *param1;

                g_TouchPanel_Sampling_Settings.SampleRate.SamplingPeriodCurrent = samplesPerSecond;
            }
            else
            {
                return CLR_E_OUT_OF_RANGE;
            }
        }
        else if (flags & TouchInfo_StylusMoveFrequency)
        {
            CLR_INT32 ms_per_touchmove_event = *param1; // *param1 is in milliseconds
            CLR_INT32 min_ms_per_touchsample = 1000 / g_TouchPanel_Sampling_Settings.SampleRate.SamplingPeriodLow;
            CLR_INT32 ticks;

            // zero value indicates turning move notifications based on time off
            if (ms_per_touchmove_event == 0)
                ticks = 0x7FFFFFFF;
            // min_ms_per_touchsample is the sample frequency for the touch screen driver
            // Touch Move events are queued up and sent at the given time frequency (StylusMoveFrequency)
            // We should not set the move frequency to be less than the sample frequency, otherwise there
            // would be no data available occassionally.
            else
            {
                ticks = TOUCH_PANEL_SAMPLE_MS_TO_TICKS(ms_per_touchmove_event);
                if (ticks < min_ms_per_touchsample)
                    return CLR_E_OUT_OF_RANGE;
            }

            g_TouchPanel_Sampling_Settings.SampleRate.MaxTimeForMoveEvent_Milliseconds = ticks;
        }
        else if (flags & TouchInfo_SamplingFilterDistance)
        {
            g_TouchDevice.MaxFilterDistance = *param1;
        }
        else
        {
            return CLR_E_INVALID_PARAMETER;
        }
    }
    else // GET
    {
        *param1 = 0;
        *param2 = 0;
        *param3 = 0;

        if (flags & TouchInfo_LastTouchPoint)
        {
            CLR_UINT16 x = 0;
            CLR_UINT16 y = 0;
            CLR_UINT32 touchFlags = GetTouchPointFlags_LatestPoint;
            CLR_INT64 time = 0;
            GetTouchPoint(&touchFlags, &x, &y, &time);
            *param1 = x;
            *param2 = y;
        }
        else if (flags & TouchInfo_SamplingDistance)
        {
            *param1 = g_TouchPanel.m_samplingPeriod;
        }
        else if (flags & TouchInfo_StylusMoveFrequency)
        {
            int ticks = g_TouchPanel_Sampling_Settings.SampleRate.MaxTimeForMoveEvent_Milliseconds;

            if (ticks == 0x7FFFFFFF)
                *param1 = 0;
            else
                *param1 = ticks / TIME_CONVERSION__TO_MILLISECONDS;
        }
        else if (flags & TouchInfo_SamplingFilterDistance)
        {
            *param1 = g_TouchDevice.MaxFilterDistance;
        }
        else
        {
            return CLR_E_INVALID_PARAMETER;
        }
    }
    return S_OK;
}

HRESULT TouchPanel::GetTouchPoint(CLR_UINT32 *flags, TouchPoint **point)
{
    CLR_UINT8 searchFlag = *flags & 0xF;
    CLR_UINT8 conditionalFlag = *flags & 0xF0;
    CLR_INT32 index = 0;

    if (g_TouchPanel.m_head == g_TouchPanel.m_tail)
    {
        return CLR_E_FAIL;
    }

    if (searchFlag == GetTouchPointFlags_LatestPoint)
    {
        index = g_TouchPanel.m_tail > 0 ? (g_TouchPanel.m_tail - 1) : (TOUCH_POINT_BUFFER_SIZE - 1);
    }
    else if (searchFlag == GetTouchPointFlags_NextPoint)
    {
        index = (*flags >> 16);
        index = (index + 1) % TOUCH_POINT_BUFFER_SIZE;
        if ((index == g_TouchPanel.m_tail))
        {
            return CLR_E_FAIL;
        }
    }

    *point = &g_PAL_TouchPointBuffer[index];

    *flags &= 0xFFFF; // Clear high 16 bit.
    *flags |= (index << 16);

    return S_OK;
}
HRESULT TouchPanel::GetTouchPoint(CLR_UINT32 *flags, CLR_UINT32 *location, CLR_INT64 *time)
{
    TouchPoint *point;

    HRESULT hr = GetTouchPoint(flags, &point);
    if (hr != S_OK)
    {
        return hr;
    }

    *location = point->location;
    *time = point->time;

    return S_OK;
}

HRESULT TouchPanel::GetTouchPoint(CLR_UINT32 *flags, CLR_UINT16 *x, CLR_UINT16 *y, CLR_INT64 *time)
{
    CLR_UINT32 location = 0;
    HRESULT hr = GetTouchPoint(flags, &location, time);
    if (hr != S_OK)
    {
        return hr;
    }
    *x = location & 0x3FFF;
    *y = (location >> 14) & 0x3FFF;

    return S_OK;
}

void TouchPanel::GetPoint(TOUCH_PANEL_SAMPLE_FLAGS *pTipState, int *pUnCalX, int *pUnCalY)
{
    *pTipState = 0;
    *pUnCalX = 0;
    *pUnCalY = 0;

    static bool stylusDown = false;
    TouchPointDevice point = g_TouchDevice.GetPoint();
    if (stylusDown)
    {
        *pTipState |= TouchSamplePreviousDownFlag;
    }
    *pUnCalX = point.x;
    *pUnCalY = point.y;
    *pTipState |= TouchSampleDownFlag;
    stylusDown = true;
}

bool TouchPanel::CalibrationPointGet(TOUCH_PANEL_CALIBRATION_POINT *pTCP)
{

    CLR_INT32 cDisplayWidth = pTCP->cDisplayWidth;
    CLR_INT32 cDisplayHeight = pTCP->cDisplayHeight;

    int CalibrationRadiusX = cDisplayWidth / 20;
    int CalibrationRadiusY = cDisplayHeight / 20;

    switch (pTCP->PointNumber)
    {
        case 0:
            pTCP->CalibrationX = cDisplayWidth / 2;
            pTCP->CalibrationY = cDisplayHeight / 2;
            break;

        case 1:
            pTCP->CalibrationX = CalibrationRadiusX * 2;
            pTCP->CalibrationY = CalibrationRadiusY * 2;
            break;

        case 2:
            pTCP->CalibrationX = CalibrationRadiusX * 2;
            pTCP->CalibrationY = cDisplayHeight - CalibrationRadiusY * 2;
            break;

        case 3:
            pTCP->CalibrationX = cDisplayWidth - CalibrationRadiusX * 2;
            pTCP->CalibrationY = cDisplayHeight - CalibrationRadiusY * 2;
            break;

        case 4:
            pTCP->CalibrationX = cDisplayWidth - CalibrationRadiusX * 2;
            pTCP->CalibrationY = CalibrationRadiusY * 2;
            break;

        default:
            pTCP->CalibrationX = cDisplayWidth / 2;
            pTCP->CalibrationY = cDisplayHeight / 2;

            return FALSE;
    }

    return TRUE;
}

#pragma endregion

#pragma region Gesture Detection

void TouchPanel::GestureDetect(CLR_INT16 x, CLR_INT16 y)
{
    bool diagonal;
    CLR_INT16 dx;
    CLR_INT16 dy;
    CLR_INT16 adx;
    CLR_INT16 ady;

    dx = x - m_lastx_gesture;
    dy = y - m_lasty_gesture;

    adx = abs(dx);
    ady = abs(dy);

    if ((adx + ady) < TOUCH_PANEL_MINIMUM_GESTURE_DISTANCE)
    {
        return;
    }

    int dir = 0;

    // Diagonal line is defined as a line with angle between 22.5 degrees and 67.5
    // (and equivalent translations in each quadrant).
    // which means the
    // abs(dx) - abs(dy) <= abs(dx) if dx > dy or abs(dy) -abs(dx) <=  abs(dy) if dy > dx

    diagonal = false;
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

    m_lastx_gesture = x;
    m_lasty_gesture = y;

    PostManagedEvent(EVENT_GESTURE, dir, 0, ((CLR_UINT32)m_x_start_gesture << 16) | m_y_start_gesture);

    return;
}
#pragma endregion

#pragma region Ink

TOUCH_PANEL_Point InkLastPoint;
bool m_initialized;
CLR_UINT32 m_index_ink;
bool m_InkingActive = false;
CLR_UINT16 m_lastx_ink;
CLR_UINT16 m_lasty_ink;
PAL_GFX_Bitmap m_ScreenBmp;

HRESULT TouchPanel::SetRegion(InkRegionInfo *inkRegionInfo)
{
    m_InkRegionInfo = *inkRegionInfo;

    /// Possibly validate m_InkRegionInfo data.
    m_InkingActive = true;
    m_lastx_ink = 0xFFFF;
    m_lasty_ink = 0xFFFF;
    m_ScreenBmp.clipping.left = m_InkRegionInfo.X1 + m_InkRegionInfo.BorderWidth;
    m_ScreenBmp.clipping.right = m_InkRegionInfo.X2 - m_InkRegionInfo.BorderWidth;
    m_ScreenBmp.clipping.top = m_InkRegionInfo.Y1 + m_InkRegionInfo.BorderWidth;
    m_ScreenBmp.clipping.bottom = m_InkRegionInfo.Y2 - m_InkRegionInfo.BorderWidth;

    if (m_InkRegionInfo.Bmp == NULL)
    {
        m_InkRegionInfo.Bmp = &m_ScreenBmp;
    }

    CLR_UINT32 flags = GetTouchPointFlags_LatestPoint;
    CLR_UINT16 source = 0;
    CLR_UINT16 x = 0;
    CLR_UINT16 y = 0;
    CLR_INT64 time = 0;

    if (GetTouchPoint(&flags, &x, &y, &time) == S_OK)
    {

        if (x == TouchPointLocationFlags_ContactUp)
            return S_OK;

        if (x == TouchPointLocationFlags_ContactDown)
        {
            GetTouchPoint(&flags, &x, &y, &time);
        }

        m_lastx_ink = x;
        m_lasty_ink = y;
    }
    else
        return CLR_E_FAIL;

    return S_OK;
}

HRESULT TouchPanel::ResetRegion()
{
    m_InkingActive = false;
    m_InkRegionInfo.Bmp = NULL;
    return S_OK;
}
void TouchPanel::DrawInk(void *arg)
{
    if (arg == NULL)
    {
    }; // Avoid unused parameter, maybe we need it in the future?
    HRESULT hr = S_OK;
    CLR_UINT32 flags = (m_index_ink << 16) | GetTouchPointFlags_NextPoint;
    CLR_UINT16 x = 0;
    CLR_UINT16 y = 0;
    CLR_INT64 time = 0;
    CLR_INT16 dx = m_InkRegionInfo.X1;
    CLR_INT16 dy = m_InkRegionInfo.Y1;

    while ((hr = GetTouchPoint(&flags, &x, &y, &time)) == S_OK)
    {
        if (x == TouchPointLocationFlags_ContactUp)
            break;
        if (x == TouchPointLocationFlags_ContactDown)
            continue;

        if (m_lastx_ink != 0xFFFF)
        {
            g_GraphicsDriver.DrawLineRaw(m_ScreenBmp, m_InkRegionInfo.Pen, m_lastx_ink, m_lasty_ink, x, y);
            if (m_InkRegionInfo.Bmp != NULL)
            {
                g_GraphicsDriver.DrawLine(
                    *(m_InkRegionInfo.Bmp),
                    m_InkRegionInfo.Pen,
                    m_lastx_ink - dx,
                    m_lasty_ink - dy,
                    x - dx,
                    y - dy);
            }
        }

        m_lastx_ink = x;
        m_lasty_ink = y;

        m_index_ink = (flags >> 16);
    }
}
#pragma endregion
