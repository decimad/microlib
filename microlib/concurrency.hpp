//          Copyright Michael Steinberg 2015
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CONCURRENCY_HPP_
#define CONCURRENCY_HPP_

/*
#include <ch.h>
#include <chbsem.h>
#include <chmtx.h>

*/

namespace ulib
{

    struct NoConcurrency
    {

        void protect()
        {
        }

        void unprotect()
        {
        }
    };
    /*
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
    */

} // namespace ulib

#endif /* CONCURRENCY_HPP_ */
