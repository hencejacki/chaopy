/*
 * File: ChaopyCopy.h
 * Description: Copy file into target folder
 * Author: HenceJacki <hencejacki@gmail.com>
 * Date: 20241202
 *
 * Revision History:
 * - 20241202: Initial creation.
 */

#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <memory>
#include "thread-pool/ThreadPool.h"
#include "../Core/ChaopyObserver.h"

class ChaopyCopy : public ChaopyObserver {
public:
	ChaopyCopy();
	virtual ~ChaopyCopy();

protected:
	virtual void update(const FileStateNotification_t&) override;

private:
	void start();
	void stop();

private:
	std::unique_ptr<ThreadPool> pool_;
	std::queue<FileStateNotification_t> task_;
	std::mutex mtx_;
	std::condition_variable cv_;
	std::atomic<bool> running_;
};