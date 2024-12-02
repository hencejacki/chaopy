/*
 * File: ChaopyObserver.h
 * Description:
 * Author: HenceJacki <hencejacki@gmail.com>
 * Date: 20241128
 *
 * Revision History:
 * - 20241128: Initial creation.
 */

#pragma once

#include "../Model/ChaopyModel.h"

class ChaopyObserver
{
public:
	ChaopyObserver() {}
	virtual ~ChaopyObserver() {}

	virtual void update(const FileStateNotification_t&) = 0;
};