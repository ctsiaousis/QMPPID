#include "QMPPID.h"

QMPPID::QMPPID() { }

#ifdef Q_OS_WIN

bool QMPPID::IS_INITIALIZED = false;
bool QMPPID::IS_RUNNING_THROUGH_CREATOR = false;

DWORD QMPPID::getParentPID(DWORD pid)
{
    HANDLE h = NULL;
    PROCESSENTRY32 pe = { 0 };
    DWORD ppid = 0;
    pe.dwSize = sizeof(PROCESSENTRY32);
    h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (Process32First(h, &pe)) {
        do {
            if (pe.th32ProcessID == pid) {
                ppid = pe.th32ParentProcessID;
                break;
            }
        } while (Process32Next(h, &pe));
    }
    CloseHandle(h);
    return (ppid);
}

int QMPPID::getProcessName(DWORD pid, QString &name)
{
    HANDLE h = NULL;
    int e = 0;
    h = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ /*| PROCESS_TERMINATE*/, FALSE, pid);
    if (h) {
        TCHAR fname[MAX_PATH];
        if (GetModuleFileNameEx(h, NULL, fname, MAX_PATH) == 0) {
            e = GetLastError();
        } else {
            name = QString::fromWCharArray(fname);
        }
        CloseHandle(h);
    } else {
        e = GetLastError();
    }
    return (e);
}

#  include <QDebug>
bool QMPPID::getIsRunningThroughCreator()
{
    if (IS_INITIALIZED) return IS_RUNNING_THROUGH_CREATOR;

    DWORD mpid = GetCurrentProcessId();
    qDebug() << "MPPID::getIsRunningThroughCreator - mpid" << mpid;
    DWORD ppid = getParentPID(mpid);
    qDebug() << "MPPID::getIsRunningThroughCreator - ppid" << ppid;

    QString name;
    int ret = getProcessName(ppid, name);
    qDebug() << "MPPID::getIsRunningThroughCreator - ret" << ret;
    qDebug() << "MPPID::getIsRunningThroughCreator - name" << name;

    IS_RUNNING_THROUGH_CREATOR = name.contains("qtcreator_processlauncher.exe", Qt::CaseInsensitive);
    IS_INITIALIZED = true;

    return IS_RUNNING_THROUGH_CREATOR;
}

#endif
