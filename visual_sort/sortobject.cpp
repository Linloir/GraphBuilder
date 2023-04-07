#include "sortobject.h"

SortObject::SortObject(QObject *parent)
    : QObject{parent}
{

}

bool SortObject::getRunFlag() const
{
    return runFlag;
}

void SortObject::setRunFlag(bool flag)
{
    if (runFlag == flag) {
        return;
    }
    runFlag = flag;
    emit runFlagChanged(flag);
}
