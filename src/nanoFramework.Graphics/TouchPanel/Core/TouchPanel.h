#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "Graphics.h"

// #define TOUCH_PANEL_SAMPLE_RATE_LOW             100
// #define TOUCH_PANEL_SAMPLE_RATE_HIGH            100
// #define RollingAverageBufferSize 8

#define TOUCH_PANEL_SAMPLE_RATE_ID                0
#define TOUCH_PANEL_SAMPLE_MS_TO_TICKS(x)         (x * TIME_CONVERSION__TO_MILLISECONDS)
#define TOUCH_PANEL_CALIBRATION_POINT_COUNT_ID    1
#define TOUCH_PANEL_CALIBRATION_POINT_ID          2
#define TOUCH_PANEL_DEFAULT_TRANSIENT_BUFFER_SIZE 100
#define TOUCH_PANEL_DEFAULT_STROKE_BUFFER_SIZE    200
#define TOUCH_PANEL_MINIMUM_GESTURE_DISTANCE      10
#define GESTURE_STATE_COUNT                       10

typedef CLR_UINT32 TOUCH_PANEL_SAMPLE_FLAGS;

struct TOUCH_PANEL_SAMPLE_RATE
{
    CLR_INT32 SamplingPeriodLow;
    CLR_INT32 SamplingPeriodHigh;
    CLR_INT32 SamplingPeriodCurrent;
    CLR_INT32 MaxTimeForMoveEvent_Milliseconds;
};

struct TOUCH_PANEL_CALIBRATION_POINT_COUNT
{
    CLR_UINT32 flags;
    CLR_INT32 cCalibrationPoints;
};

struct TOUCH_PANEL_CALIBRATION_POINT
{
    CLR_INT32 PointNumber;
    CLR_INT32 cDisplayWidth;
    CLR_INT32 cDisplayHeight;
    CLR_INT32 CalibrationX;
    CLR_INT32 CalibrationY;
};

enum TouchPanel_SampleFlags
{
    TouchSampleDownFlag = 0x02,
    TouchSampleIsCalibratedFlag = 0x04,
    TouchSamplePreviousDownFlag = 0x08,
    TouchSampleIgnore = 0x10,
    TouchSampleMouse = 0x40000000
};

enum TouchPanel_StylusFlags
{
    TouchPanelStylusInvalid = 0x0,
    TouchPanelStylusDown = 0x1,
    TouchPanelStylusUp = 0x2,
    TouchPanelStylusMove = 0x3,
};

enum TouchPanel_TouchCollectorFlags
{
    TouchCollector_EnableCollection = 0x1,
    TouchCollector_DirectInk = 0x2,
    TouchCollector_GestureRecognition = 0x4,
};

enum TouchPanel_TouchInfoFlags
{
    TouchInfo_Set = 0x1,
    TouchInfo_LastTouchPoint = 0x2,
    TouchInfo_SamplingDistance = 0x4,
    TouchInfo_StylusMoveFrequency = 0x8,
    TouchInfo_SamplingFilterDistance = 0x40,
};

enum TouchPointLocationFlags
{
    TouchPointLocationFlags_ContactDown = 0x3FF,
    TouchPointLocationFlags_ContactUp = 0x3FE,
};

enum TouchPointContactFlags
{
    TouchPointContactFlags_Primary = 0x80000000,
    TouchPointContactFlags_Pen = 0x40000000,
    TouchPointContactFlags_Palm = 0x20000000,
};

enum GetTouchPointFlags
{
    GetTouchPointFlags_LatestPoint = 0x0,
    GetTouchPointFlags_NextPoint = 0x1,
};

enum TouchGestures
{
    TouchGesture_NoGesture = 0, // Can be used to represent an error gesture or unknown gesture
    TouchGesture_Begin = 1,     // Used to identify the beginning of a Gesture Sequence; App can use this to highlight
                                // UIElement or some other sort of notification.
    TouchGesture_End =
        2, // Used to identify the end of a gesture sequence; Fired when last finger involved in a gesture is removed.

    // Standard stylus (single touch) gestues
    TouchGesture_Right = 3,
    TouchGesture_UpRight = 4,
    TouchGesture_Up = 5,
    TouchGesture_UpLeft = 6,
    TouchGesture_Left = 7,
    TouchGesture_DownLeft = 8,
    TouchGesture_Down = 9,
    TouchGesture_DownRight = 10,
    TouchGesture_Tap = 11,
    TouchGesture_DoubleTap = 12,
};

struct TOUCH_PANEL_CalibrationData
{
    CLR_INT32 Mx;
    CLR_INT32 My;

    CLR_INT32 Cx;
    CLR_INT32 Cy;

    CLR_INT32 Dx;
    CLR_INT32 Dy;
};

struct TOUCH_PANEL_SamplingSettings
{
    bool ActivePinStateForTouchDown;
    TOUCH_PANEL_SAMPLE_RATE SampleRate;
};

struct TouchPoint
{
    /// Location is a composite of  bits 0-13: x, 14-27: y
    CLR_UINT32 location;
    /// Contact is a composite of  bits 0-13: width, 14-27: height, 28-31: flags
    CLR_UINT32 contact;
    CLR_INT64 time;
};

struct TOUCH_PANEL_Point
{
    CLR_UINT16 sx;
    CLR_UINT16 sy;
};

struct TOUCH_PANEL_TouchCollectorData
{
    /// ISSUE: Use something like private alloc here?
    /// For now these are static buffers.
    TOUCH_PANEL_Point TransientBuffer[TOUCH_PANEL_DEFAULT_TRANSIENT_BUFFER_SIZE];
    TOUCH_PANEL_Point StrokeBuffer[TOUCH_PANEL_DEFAULT_STROKE_BUFFER_SIZE];
    unsigned char TouchCollectorFlags;
    CLR_INT32 TouchCollectorX1;
    CLR_INT32 TouchCollectorX2;
    CLR_INT32 TouchCollectorY1;
    CLR_INT32 TouchCollectorY2;
    Hal_Queue_UnknownSize<TOUCH_PANEL_Point> TransientPointBuffer;
    Hal_Queue_UnknownSize<TOUCH_PANEL_Point> StrokePointBuffer;
};
struct InkRegionInfo
{
    CLR_UINT16 X1, X2, Y1, Y2; /// Inking region in screen co-ordinates.
    CLR_UINT16 BorderWidth;    /// border width for inking region
    PAL_GFX_Bitmap *Bmp;       /// This field may be NULL, if not NULL it must be valid pinned memory.
                               /// Other criterion is this bitmap must have size (X2-X1) x (Y2-Y1).
    GFX_Pen Pen;
};

class TouchPanel
{
  public:
    static HRESULT Initialize();
    static HRESULT Uninitialize();
    static HRESULT GetDeviceCaps(unsigned int iIndex, void *lpOutput);
    static HRESULT ResetCalibration();
    static HRESULT SetCalibration(int pointCount, CLR_INT16 *sx, CLR_INT16 *sy, CLR_INT16 *ux, CLR_INT16 *uy);
    static HRESULT GetTouchPoints(int *pointCount, CLR_INT16 *sx, CLR_INT16 *sy);
    static HRESULT GetSetTouchInfo(CLR_UINT32 flags, CLR_INT32 *param1, CLR_INT32 *param2, CLR_INT32 *param3);
    static HRESULT GetTouchPoint(CLR_UINT32 *flags, CLR_UINT16 *x, CLR_UINT16 *y, CLR_INT64 *time);
    static HRESULT GetTouchPoint(CLR_UINT32 *flags, CLR_UINT32 *location, CLR_INT64 *time);
    static void GestureDetect(CLR_INT16 x, CLR_INT16 y);

    static HRESULT SetRegion(InkRegionInfo *inkRegionInfo);
    static HRESULT ResetRegion();

    static void DrawInk(void *arg);

    TOUCH_PANEL_SAMPLE_RATE SampleRate;

    void GetPoint(TOUCH_PANEL_SAMPLE_FLAGS *pTipState, int *pUnCalX, int *pUnCalY);
    bool CalibrationPointGet(TOUCH_PANEL_CALIBRATION_POINT *pTCP);

  private:
    static HRESULT GetTouchPoint(CLR_UINT32 *flags, TouchPoint **point);

    static void TouchIsrProc(GPIO_PIN pin, bool pinState, void *pArg);
    static void TouchCompletion(void *arg);
    void TouchPanelCalibratePoint(CLR_INT32 UncalX, CLR_INT32 UncalY, CLR_INT32 *pCalX, CLR_INT32 *pCalY);

    void PollTouchPoint();
    TouchPoint *AddTouchPoint(CLR_UINT16 x, CLR_UINT16 y, CLR_INT64 time);

    int m_touchMoveIndex;
    TouchPoint *m_startMovePtr;
    HAL_COMPLETION m_touchCompletion;
    TOUCH_PANEL_CalibrationData m_calibrationData;
    CLR_INT32 m_samplingPeriod;
    CLR_INT32 m_InternalFlags;

    CLR_INT32 m_head;
    CLR_INT32 m_tail;

    enum InternalFlags
    {
        Contact_Down = 0x1,
        Contact_WasDown = 0x2,
    };
};
