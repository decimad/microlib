/*
 * concurrency.hpp
 *
 *  Created on: 22.12.2014
 *      Author: Michael
 */

#ifndef CONCURRENCY_HPP_
#define CONCURRENCY_HPP_
#include <ch.h>
#include <chmtx.h>
#include <chbsem.h>

namespace util {

	struct NoConcurrency {

		void protect() {

		}

		void unprotect() {

		}

	};

	struct SysLock {

		void protect() {
			chSysLock();
		}

		void unprotect() {
			chSysUnlock();
		}

	};

	struct Mutex {
		Mutex()
		{
			chMtxInit(&mut_);
		}



		::Mutex mut_;
	};

	struct BinarySem {
		BinarySem()
		{
			chBSemInit(&sema_, 0);
		}

		void signal() {
			chBSemSignal(&sema_);
		}

		void wait() {
			chBSemWait(&sema_);
		}

		void reset() {
			chBSemReset(&sema_, 0);
		}

		::BinarySemaphore sema_;
	};


}



#endif /* CONCURRENCY_HPP_ */
