#pragma once
class Timer
{
public:
	Timer();
	float DeltaTime()const;
	float UnScaledDeltaTime()const;
	float FixedDelta()const;
	void SetFixedDelta(float dt);
	double Elapsed()const;
	double UnscaledElapsed()const;
	__int64 TimeCounter()const;
	__int64 UnscaledTimeCounter()const;
	void SetTimeScale(float timeScale) { m_timeScale = timeScale; };
	void Reset();
	void Tick();
private:
	double m_secondPerCount;
	__int64 m_startTime;
	__int64 m_prevTime;
	__int64 m_currTime;
	__int64 m_totalTime;
	__int64 m_unscaledTotalTime;
	double m_deltaTime;
	double m_unscaledDeltaTime;
	float m_timeScale;
	float m_fixedDelta;
};

extern Timer g_Timer;