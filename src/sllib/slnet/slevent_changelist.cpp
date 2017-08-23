#include "slevent_changelist.h"
#include "sltools.h"
#include "slevent_dispatcher.h"
namespace sl{
namespace network{

EventChange* EventChangeList::EventChangeListGetOrConstruct(int32 fd, int16 oldEvents, EventChangeListFdInfo *fdinfo){
	EventChange* change = nullptr;

	if (fdinfo->idxplus1 == 0){
		int32 idx;
		SLASSERT(_changesNum <= _changesSize, "wtf");

		if (_changesNum == _changesSize){
			if (ChangeListGrow() < 0)//À©ÈÝ
				return nullptr;
		}

		idx = _changesNum++;
		change = &_changes[idx];
		fdinfo->idxplus1 = idx + 1;

		sl::SafeMemset(change, sizeof(EventChange), 0, sizeof(EventChange));
		change->fd = fd;
		change->oldEvents = oldEvents;
	}
	else{
		change = &_changes[fdinfo->idxplus1 - 1];
		SLASSERT(change->fd == fd, "wrf");
	}

	return change;
}

int32 EventChangeList::EventChangeListAdd(int32 fd, int16 old, int16 events, void* fdinfo){
	EventChangeListFdInfo *tfdinfo = (EventChangeListFdInfo *)fdinfo;
	EventChange* change;

	change = EventChangeListGetOrConstruct(fd, old, tfdinfo);
	if (!change)
		return -1;

	if (events & (EV_READ | EV_SIGNAL)) {
		change->readChange = EV_CHANGE_ADD |
			(events & (EV_ET | EV_PERSIST | EV_SIGNAL));
	}

	if (events & EV_WRITE) {
		change->writeChange = EV_CHANGE_ADD |
			(events & (EV_ET | EV_PERSIST | EV_SIGNAL));
	}

	if (events & EV_CLOSED) {
		change->closeChange = EV_CHANGE_ADD |
			(events & (EV_ET | EV_PERSIST | EV_SIGNAL));
	}
}

int32 EventChangeList::EventChangeListDel(int32 fd, int16 old, int16 events, void* fdinfo){
	EventChangeListFdInfo* tfdinfo = (EventChangeListFdInfo*)fdinfo;
	EventChange* change;

	change = EventChangeListGetOrConstruct(fd, old, tfdinfo);
	if (!change)
		return -1;

	if (events & (EV_READ | EV_SIGNAL)){
		if (!(change->oldEvents & (EV_READ | EV_SIGNAL)))
			change->readChange = 0;
		else
			change->readChange = EV_CHANGE_DEL;
	}

	if (events & EV_WRITE){
		if (!(change->oldEvents & EV_WRITE))
			change->writeChange = 0;
		else
			change->writeChange = EV_CHANGE_DEL;
	}
	if (events & EV_CLOSED){
		if (!(change->oldEvents & EV_CLOSED))
			change->closeChange = 0;
		else
			change->closeChange = EV_CHANGE_DEL;
	}

	return 0;
}

int32 EventChangeList::ChangeListGrow(){
	int32 newSize;
	EventChange* newChanges;
	if (_changesSize < 64)
		newSize = 64;
	else
		newSize = _changesSize * 2;

	newChanges = (EventChange*)SLREALLOC(_changes, newSize * sizeof(EventChange));

	if (newChanges == nullptr)
		return -1;

	_changes = newChanges;
	_changesSize = newSize;

	return 0;
}

void EventChangeList::EventChangeListRemoveAll(){
	for (int32 i = 0; i < _changesNum; ++i) {
		EventChange *ch = &_changes[i];
		
		EventIO* ioInfo =  _eventDispatcher->getEventIOInfo(ch->fd);
		SLASSERT(ioInfo->channelListIdx == i + 1, "wtf");
		ioInfo->channelListIdx = 0;
	}

	_changesNum = 0;
}


}
}
