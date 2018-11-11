#ifndef	___HPP_MOVE_FUN_DEF___
#define ___HPP_MOVE_FUN_DEF___

template<class T>

class MoveFun
{
private:
	T m_rel; //速度
	T m_pos;//目前位置
	T m_aimpos;//结束位置

public:
	MoveFun(void){}
	~MoveFun(void){}

	void Init(T start, T end)
	{
		m_rel = 0.0f;
		m_pos = start;
		m_aimpos = end;
	}

	T lerp(int currentFrame, int totalframe)
	{
		T pos = 0.0f;
		if (totalframe <= 0)
		{
			return pos;
		}
		T rat = (currentFrame) / (T)(totalframe);
		T uint = 1.0f / totalframe;
		pos = SmoothCD(m_pos, m_aimpos, m_rel, 1.0f-rat, uint);
		pos += m_rel*uint;
		m_pos = pos;
		return m_pos;
	}

	T SmoothCD(const T& a_From, const T& a_To, T& a_Vel, T a_SmoothTime, T a_DeltaTime)
	{
		if (a_SmoothTime<=0)
		{
			return 0;
		}
		T omega = 2.0f/a_SmoothTime;
		T x = omega*a_DeltaTime;
		T exp = 1.0f/(1.0f + x + 0.48f* x * x + 0.235f * x * x * x);
		T change = a_From - a_To;
		T temp = (a_Vel + omega * change)*a_DeltaTime;
		a_Vel = (a_Vel - omega * temp)*exp;
		return a_To + (change + temp)*exp;
	}
};


#endif