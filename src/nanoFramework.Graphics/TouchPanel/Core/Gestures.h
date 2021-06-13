////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Graphics.h>


struct GestureDriver
{
    static const int c_IgnoreCount = 2;

  private:
    static bool s_initialized;

    PalEventListener m_gestureListener;
    HAL_COMPLETION m_gestureCompletion;
    CLR_UINT32 m_index;
    CLR_UINT32 m_currentState;
    CLR_UINT16 m_lastx;
    CLR_UINT16 m_lasty;
    CLR_UINT16 m_startx;
    CLR_UINT16 m_starty;

    CLR_UINT32 m_stateIgnoreIndex;
    CLR_UINT32 m_stateIgnoreHead;
    CLR_UINT32 m_stateIgnoreTail;
    CLR_UINT32 m_stateIgnoreBuffer[c_IgnoreCount];

  public:
    static HRESULT Initialize();
    static HRESULT Uninitialize();
    static bool ProcessPoint(CLR_UINT32 flags, CLR_UINT16 source, CLR_UINT16 x, CLR_UINT16 y, CLR_INT64 time);

    static void ResetRecognition();
    static void EventListener(CLR_UINT32 e, CLR_UINT32 param);
    static void GestureContinuationRoutine(void *arg);
};


//enum TouchGestures
//{
//    TouchGesture_NoGesture = 0, // Can be used to represent an error gesture or unknown gesture
//
//    // Standard Win7 Gestures
//    TouchGesture_Begin = 1, // Used to identify the beginning of a Gesture Sequence; App can use this to highlight
//                            // UIElement or some other sort of notification.
//    TouchGesture_End =
//        2, // Used to identify the end of a gesture sequence; Fired when last finger involved in a gesture is removed.
//
//    // Standard stylus (single touch) gestues
//    TouchGesture_Right = 3,
//    TouchGesture_UpRight = 4,
//    TouchGesture_Up = 5,
//    TouchGesture_UpLeft = 6,
//    TouchGesture_Left = 7,
//    TouchGesture_DownLeft = 8,
//    TouchGesture_Down = 9,
//    TouchGesture_DownRight = 10,
//    TouchGesture_Tap = 11,
//    TouchGesture_DoubleTap = 12,
//
//    // Multi-touch gestures
//    TouchGesture_Zoom = 114, // Equivalent to your "Pinch" gesture
//    TouchGesture_Pan = 115,  // Equivalent to your "Scroll" gesture
//    TouchGesture_Rotate = 116,
//    TouchGesture_TwoFingerTap = 117,
//    TouchGesture_Rollover = 118, // Press and tap
//
//    // Additional NetMF gestures
//    TouchGesture_UserDefined = 200
//};


