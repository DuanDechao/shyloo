namespace sl{
inline TimeBase::TimeBase(TimersBase& Owner, TimerHandler* pHandler, void* pUserData)
	:m_Owner(Owner),
	 m_pHandler(pHandler),
	 m_pUserData(pUserData),
	 m_stState(TIME_PENDING)
{
	SL_ASSERT(pHandler != NULL);
	pHandler->incTimerRegisterCount();
}

inline void TimeBase::cancel()
{
	if(this->isCancelled())
	{
		return;
	}
	SL_ASSERT((m_stState == TIME_PENDING) || (m_stState == TIME_EXECUTING));
	m_stState = TIME_CANCELLED;

	if(m_pHandler)
	{
		m_pHandler->release(TimerHandle(this), m_pUserData);
		m_pHandler = NULL;
	}
	m_Owner.onCancel();
}

inline void TimerHandle::cancel()
{
	if(NULL != m_pTime)
	{
		TimeBase* pTime = m_pTime;
		m_pTime = NULL;
		pTime->cancel();
	}
}
}