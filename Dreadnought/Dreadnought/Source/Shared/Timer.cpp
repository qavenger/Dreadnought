#include "pch.h"
#include "Timer.h"

Timer g_Timer;
Timer::Timer() :
	m_deltaTime(0),
	m_secondPerCount(0),
	m_startTime(0),
	m_prevTime(0),
	m_totalTime(0),
	m_unscaledTotalTime(0),
	m_currTime(0),
	m_timeScale(1)
{
	__int64 countPerSecond;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countPerSecond);
	m_secondPerCount = 1 / (double)countPerSecond;
}

float Timer::DeltaTime() const
{
	return (float)m_deltaTime;
}

float Timer::UnScaledDeltaTime() const
{
	return (float)m_unscaledDeltaTime;
}

float Timer::FixedDelta() const
{
	return m_fixedDelta;
}

void Timer::SetFixedDelta(float dt)
{
	m_fixedDelta = dt;
}

double Timer::Elapsed() const
{
	return m_totalTime * m_secondPerCount;
}

double Timer::UnscaledElapsed() const
{
	return m_unscaledTotalTime * m_secondPerCount;
}

__int64 Timer::TimeCounter() const
{
	return m_totalTime;
}

__int64 Timer::UnscaledTimeCounter() const
{
	return m_unscaledTotalTime;
}

void Timer::Reset()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&m_startTime);
	m_prevTime = m_currTime = m_startTime;
	m_timeScale = 1;
	m_deltaTime = 0;
	m_unscaledTotalTime = 0;
	m_totalTime = 0;
}

void Timer::Tick()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&m_currTime);
	__int64 dt = max(0,m_currTime - m_prevTime);

	m_unscaledTotalTime += dt;
	m_unscaledDeltaTime = dt * m_secondPerCount;

	m_totalTime += (__int64)(dt * m_timeScale);
	m_deltaTime = dt * m_secondPerCount * m_timeScale;

	m_prevTime = m_currTime;
}
