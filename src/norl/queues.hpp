#pragma once

#include "moodycamel/blockingconcurrentqueue.h"
#include "atomic_queue/atomic_queue.h"

#include <cstdlib>
#include <iostream>

template <typename T>
class Queue {
public:
	virtual void pushA(const T &t) = 0;
	virtual void pushB(const T &t) = 0;
	virtual void pop(T &) = 0;
};

template <typename T>
class MCQueue : public Queue<T> {
	moodycamel::BlockingConcurrentQueue<T> q;

public:
	void pushA(const T &t) override {
		q.enqueue(t);
	}
	void pushB(const T &t) override {
		q.enqueue(t);
	}
	void pop(T &t) override {
		q.wait_dequeue(t);
	}
};

template <typename T>
class ATQueue : public Queue<T> {
	atomic_queue::AtomicQueue2<T, 64> q;

public:
	void pushA(const T &t) override {
		q.push(t);
	}
	void pushB(const T &t) override {
		q.push(t);
	}
	void pop(T &t) override {
		t = q.pop();
	}
};
