//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "Graphics.h"
#include "TouchDevice.h"
#include "nanoFramework_Graphics.h"

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_TouchEventProcessor::
    ProcessEvent___nanoFrameworkRuntimeEventsBaseEvent__U4__U4__SystemDateTime(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    hr = S_OK;

    CLR_UINT32 data1 = stack.Arg1().NumericByRef().u4;
    CLR_UINT32 numTouches = data1 >> 16;
    TouchPoint *touchPoint = (TouchPoint *)stack.Arg2().NumericByRef().u4;
    CLR_RT_HeapBlock *touchEvent = NULL;
    CLR_RT_HeapBlock_Array *touchInputArray = NULL;
    CLR_RT_HeapBlock *touchInputObject = NULL;

    // Create a nanoFramework.UI.TouchEvent object to return
    CLR_RT_HeapBlock &resultObject = stack.PushValue();

    NANOCLR_CHECK_HRESULT(
        g_CLR_RT_ExecutionEngine.NewObjectFromIndex(resultObject, g_CLR_RT_WellKnownTypes.m_TouchEvent));

    touchEvent = resultObject.Dereference();
    if (!touchEvent)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    // Initialize the fields of the TouchEvent object.
    touchEvent[Library_nf_rt_events_native_nanoFramework_Runtime_Events_BaseEvent::FIELD__Source].SetInteger((CLR_UINT16)0);
    touchEvent[Library_nanoFramework_Graphics_nanoFramework_UI_Input_RawTouchInputReport::FIELD__EventMessage].SetInteger((CLR_UINT8)data1 & 0x00ff);

    // Add the number of 100 nanoSeconds at 01/01/1601 to the to time
    // to get past a test in the C# datetime test that only accepts dates greater than 01/01/1601
    touchEvent[Library_nanoFramework_Graphics_nanoFramework_UI_TouchEvent::FIELD__Time].SetInteger(stack.Arg3().NumericByRef().s8 + 504911232000000000);
    touchEvent[Library_nanoFramework_Graphics_nanoFramework_UI_TouchEvent::FIELD__Time].ChangeDataType(DATATYPE_DATETIME);

    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(touchEvent[Library_nanoFramework_Graphics_nanoFramework_UI_TouchEvent::FIELD__Touches],numTouches,g_CLR_RT_WellKnownTypes.m_TouchInput));

    touchInputArray =touchEvent[Library_nanoFramework_Graphics_nanoFramework_UI_TouchEvent::FIELD__Touches].DereferenceArray();
    if (!touchInputArray)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    // Marshall the TouchPoint array from the PAL into the TouchInput[] managed array.
    touchInputObject = (CLR_RT_HeapBlock *)touchInputArray->GetFirstElement();
    for (; numTouches > 0; --numTouches, touchInputObject++, touchPoint++)
    {
        g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*touchInputObject, g_CLR_RT_WellKnownTypes.m_TouchInput);
        CLR_RT_HeapBlock *touchInput = touchInputObject->Dereference();
        if (!touchInput)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }

        CLR_UINT32 location = touchPoint->location;
        touchInput[Library_nanoFramework_Graphics_nanoFramework_UI_TouchInput::FIELD__X].SetInteger((CLR_INT32)(location & 0x3fff));
        touchInput[Library_nanoFramework_Graphics_nanoFramework_UI_TouchInput::FIELD__Y].SetInteger((CLR_INT32)((location >> 14) & 0x3fff));

    }

    NANOCLR_NOCLEANUP();
}
