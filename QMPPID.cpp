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

#ifdef Q_OS_LINUX

bool QMPPID::IS_INITIALIZED = false;
bool QMPPID::IS_RUNNING_THROUGH_CREATOR = false;

pid_t QMPPID::getParentPID(pid_t pid)
{
    pid_t ppid = -1;
    char buffer[BUFSIZ];
    sprintf(buffer, "/proc/%d/stat", pid);
    FILE *fp = fopen(buffer, "r");
    if (fp) {
        size_t size = fread(buffer, sizeof(char), sizeof(buffer), fp);
        if (size > 0) {
            strtok(buffer, " ");
            strtok(NULL, " ");
            strtok(NULL, " ");
            char *s_ppid = strtok(NULL, " ");
            ppid = atoi(s_ppid);
        }
        fclose(fp);
    }
    return ppid;
}

int QMPPID::getProcessName(pid_t pid, QString &name)
{
    char mname[BUFSIZ];
    char procfile[BUFSIZ];
    sprintf(procfile, "/proc/%d/cmdline", pid);
    FILE *f = fopen(procfile, "r");
    if (!f) {
        return 1;
    }
    size_t size;
    size = fread(mname, sizeof(char), sizeof(procfile), f);
    if (size > 0) {
        if ('\n' == mname[size - 1]) mname[size - 1] = '\0';
    }
    fclose(f);
    name = QString(mname);
    return 0;
}

#  include <QDebug>
bool QMPPID::getIsRunningThroughCreator()
{
    if (IS_INITIALIZED) return IS_RUNNING_THROUGH_CREATOR;

    pid_t mpid = getpid();
    qDebug() << "MPPID::getIsRunningThroughCreator - mpid" << mpid;
    pid_t ppid = getParentPID(mpid);
    qDebug() << "MPPID::getIsRunningThroughCreator - ppid" << ppid;

    QString name;
    int ret = getProcessName(ppid, name);
    qDebug() << "MPPID::getIsRunningThroughCreator - ret" << ret;
    qDebug() << "MPPID::getIsRunningThroughCreator - name" << name;

    IS_RUNNING_THROUGH_CREATOR = name.contains("qtcreator_process", Qt::CaseInsensitive);
    IS_INITIALIZED = true;

    return IS_RUNNING_THROUGH_CREATOR;
}

#endif
