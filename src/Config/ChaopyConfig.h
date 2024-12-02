#pragma once

#include "pch.h"

typedef struct ChaopyConfig
{
	std::string path;
	std::string target;
	bool recursive;

	ChaopyConfig()
		: path("")
		, target("")
		, recursive(false)
	{

	}

	ChaopyConfig& operator =(const ChaopyConfig& other)
	{
		path = other.path;
		target = other.target;
		recursive = other.recursive;

		return *this;
	}

	ChaopyConfig(const ChaopyConfig& other)
	{
		operator =(other);
	}

}ChaopyConfig_t;
