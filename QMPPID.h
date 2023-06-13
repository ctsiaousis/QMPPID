#ifndef QMPPID_H
#define QMPPID_H

#include <QtGlobal>
#include <QString>

#if defined(Q_OS_WIN)

#  include <Windows.h>
#  include <tlhelp32.h>
#  include <Psapi.h>

class QMPPID
{
    QMPPID();
    static bool IS_INITIALIZED;
    static bool IS_RUNNING_THROUGH_CREATOR;
    static DWORD getParentPID(DWORD pid);
    static int getProcessName(DWORD pid, QString &name);

public:
    static bool getIsRunningThroughCreator();
};

#elif defined(Q_OS_LINUX)

#  include <stdio.h>
#  include <unistd.h>
#  include <string.h>
#  include <stdlib.h>

class QMPPID
{
    QMPPID();
    static bool IS_INITIALIZED;
    static bool IS_RUNNING_THROUGH_CREATOR;
    static pid_t getParentPID(pid_t pid);
    static int getProcessName(pid_t pid, QString &name);

public:
    static bool getIsRunningThroughCreator();
};

#else

class MPPID
{
public:
    MPPID();
};

#endif

#endif // QMPPID_H
