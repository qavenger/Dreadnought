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
	int64 TimeCounter()const;
	int64 UnscaledTimeCounter()const;
	void SetTimeScale(float timeScale) { m_timeScale = timeScale; };
	void Reset();
	void Tick();
private:
	double m_secondPerCount;
	int64 m_startTime;
	int64 m_prevTime;
	int64 m_currTime;
	int64 m_totalTime;
	int64 m_unscaledTotalTime;
	double m_deltaTime;
	double m_unscaledDeltaTime;
	float m_timeScale;
	float m_fixedDelta;
};
