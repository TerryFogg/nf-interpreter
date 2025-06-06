//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef NANOPAL_ASYNCPROCCALLS_DECL_H
#define NANOPAL_ASYNCPROCCALLS_DECL_H

#include <nanoWeak.h>

//--//

typedef void (*HAL_CALLBACK_FPN)(void *arg);

struct HAL_CALLBACK
{
  public:
    HAL_CALLBACK_FPN EntryPoint;
    void *Argument;

  public:
    void Initialize(HAL_CALLBACK_FPN entryPoint, void *arg)
    {
        this->EntryPoint = entryPoint;
        this->Argument = arg;
    }

    void SetArgument(void *arg)
    {
        this->Argument = arg;
    }

    HAL_CALLBACK_FPN GetEntryPoint() const
    {
        return this->EntryPoint;
    }
    void *GetArgument() const
    {
        return this->Argument;
    }

    void Execute() const
    {
        HAL_CALLBACK_FPN entryPoint = this->EntryPoint;
        void *arg = this->Argument;

        if (entryPoint)
        {
            EntryPoint(arg);
        }
    }
};

struct HAL_CONTINUATION : public HAL_DblLinkedNode<HAL_CONTINUATION>
{

  private:
    HAL_CALLBACK Callback;

  public:
    void InitializeCallback(HAL_CALLBACK_FPN EntryPoint, void *Argument = NULL);

    void SetArgument(void *Argument)
    {
        Callback.SetArgument(Argument);
    }

    HAL_CALLBACK_FPN GetEntryPoint() const
    {
        return Callback.GetEntryPoint();
    }
    void *GetArgument() const
    {
        return Callback.GetArgument();
    }

    void Execute() const
    {
        Callback.Execute();
    }

    bool IsLinked();
    void Enqueue();
    void Abort();

    //--//

    static void Uninitialize();

    static void InitializeList();

    static bool Dequeue_And_Execute();
};

//--//

struct HAL_COMPLETION : public HAL_CONTINUATION
{
    uint64_t EventTimeTicks;
    bool ExecuteInISR;

#if defined(_DEBUG)
    uint64_t Start_RTC_Ticks;
#endif

    void InitializeForISR(HAL_CALLBACK_FPN EntryPoint, void *Argument = NULL)
    {
        ExecuteInISR = true;

        InitializeCallback(EntryPoint, Argument);
    }

    void InitializeForUserMode(HAL_CALLBACK_FPN EntryPoint, void *Argument = NULL)
    {
        ExecuteInISR = false;

        InitializeCallback(EntryPoint, Argument);
    }

    void EnqueueTicks(uint64_t eventTimeTicks);
    void EnqueueDelta(uint32_t miliSecondsFromNow);

    void Abort();

    void Execute();

    //--//

    static void Uninitialize();

    static void InitializeList();

    static void DequeueAndExec();

    static void WaitForInterrupts(uint64_t expireTimeInSysTicks, uint32_t sleepLevel, uint64_t wakeEvents);
};

//--//

#endif // NANOPAL_ASYNCPROCCALLS_DECL_H
