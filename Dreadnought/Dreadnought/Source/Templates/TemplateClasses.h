#pragma once
class INoncopyable
{
protected:
	INoncopyable() {}
	~INoncopyable() {}
private:
	INoncopyable(const INoncopyable&) = delete;
	INoncopyable operator=(const INoncopyable&) = delete;
};