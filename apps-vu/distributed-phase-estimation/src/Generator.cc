//
// Created by istvan on 11/11/16.
//

#include "Generator.h"
#include <capnp/serialize.h>
#include <capnp/message.h>

#define SAMPLING_RATE 1000 // Hz
#define SIGNAL_FREQ 60 // Hz
#define PWM_PERIOD 4000 // nanoseconds

#define DPHASE (2 * M_PI * SIGNAL_FREQ / SAMPLING_RATE)

#define PWM_OUTPUT_CHIP 0
#define PWM_CHIP_OUTPUT 0

namespace timertest{
    namespace components {

        Generator::Generator(_component_conf_j &config, riaps::Actor &actor) : GeneratorBase(config, actor) {

            int policy = SCHED_RR;
            struct sched_param params;
            params.sched_priority = 98;

            int prior_pol = sched_setscheduler(0,policy,&params);
            if (prior_pol == -1){
              std::cout << "SETSCHEDULER ERROR" << std::endl;

              int errsv = errno;
              if(errsv == EINVAL) std::cout << "EINVAL" << std::endl;
              if(errsv == EPERM) std::cout << "EPERM" << std::endl;
              if(errsv == ESRCH) std::cout << "ESRCH" << std::endl;
            }


            _pwm_output = libsoc_pwm_request(PWM_OUTPUT_CHIP, PWM_CHIP_OUTPUT, LS_PWM_SHARED);
            if (!_pwm_output) {
                perror("unable to request PWM pin:");
                fprintf(stderr, "make sure, you enabled the PWM overlay:\n\techo BB-PWM1 >  /sys/devices/platform/bone_capemgr/slots\n");
                exit(-1);
            }

            libsoc_pwm_set_enabled(_pwm_output, ENABLED);
            sleep(1);
            if (!libsoc_pwm_get_enabled(_pwm_output))
            {
                perror("unable to enable PWM pin:");
                exit(-1);
            }


            libsoc_pwm_set_polarity(_pwm_output, NORMAL);
            if (libsoc_pwm_get_polarity(_pwm_output) != NORMAL)
            {
                perror("unable to set PWM polarity:");
                exit(-1);
            }

            libsoc_pwm_set_period(_pwm_output, PWM_PERIOD);
            if (libsoc_pwm_get_period(_pwm_output) != PWM_PERIOD)
            {
                perror("unable to set PWM period:");
                exit(-1);
            }
        }

        // With RIAPS timer
        void Generator::OnClock(riaps::ports::PortBase *port) {
            // std::cout << "OnClock()" <<std::endl;
            float currentValue = sin(_phase);
            _phase+=DPHASE;

            capnp::MallocMessageBuilder messageBuilder;
            auto msgSignalValue = messageBuilder.initRoot<messages::SignalValue>();
            msgSignalValue.setVal(currentValue);
            auto msgTimeStamp   = msgSignalValue.initTimestamp();

            timespec
                      t1Spec
                    //, t2Spec
                    //, tAvg
                    ;

            libsoc_pwm_set_duty_cycle(_pwm_output, PWM_PERIOD * (1.0 + currentValue) / 2.0 );
            clock_gettime(CLOCK_REALTIME, &t1Spec);
            //clock_gettime(CLOCK_REALTIME, &t2Spec);
            //tAvg.tv_nsec = (t1Spec.tv_nsec + t2Spec.tv_nsec)/2.0;
            //tAvg.tv_sec  = (t1Spec.tv_sec  + t2Spec.tv_sec)/2.0;

            //msgTimeStamp.setNsec(tAvg.tv_nsec);
            //msgTimeStamp.setSec(tAvg.tv_sec);
            msgTimeStamp.setNsec(t1Spec.tv_nsec);
            msgTimeStamp.setSec(t1Spec.tv_sec);

            SendSignalValue(messageBuilder, msgSignalValue);

        }

// --> Without RIAPS timer <--
//        void Generator::OnClock(riaps::ports::PortBase *port) {
//            auto now = std::chrono::high_resolution_clock::now();
//            _cycle=0;
//            while (true) {
//                float currentValue = sin(_phase);
//                _phase += DPHASE;
//                capnp::MallocMessageBuilder messageBuilder;
//                auto msgSignalValue = messageBuilder.initRoot<messages::SignalValue>();
//                auto msgTimeStamp = msgSignalValue.initTimestamp();
//                timespec
//                        t1Spec
//                //, t2Spec
//                //, tAvg
//                ;
//
//
//                auto diff = std::chrono::milliseconds(1);
//                now+=diff;
//
//                std::this_thread::sleep_until(now);
//
//
//
//
//                msgSignalValue.setVal(currentValue);
//
//
//
//
//                libsoc_pwm_set_duty_cycle(_pwm_output, PWM_PERIOD * (1.0 + currentValue) / 2.0);
//                clock_gettime(CLOCK_REALTIME, &t1Spec);
//                //clock_gettime(CLOCK_REALTIME, &t2Spec);
//                //tAvg.tv_nsec = (t1Spec.tv_nsec + t2Spec.tv_nsec)/2.0;
//                //tAvg.tv_sec  = (t1Spec.tv_sec  + t2Spec.tv_sec)/2.0;
//
//                //msgTimeStamp.setNsec(tAvg.tv_nsec);
//                //msgTimeStamp.setSec(tAvg.tv_sec);
//                msgTimeStamp.setNsec(t1Spec.tv_nsec);
//                msgTimeStamp.setSec(t1Spec.tv_sec);
//
//                SendSignalValue(messageBuilder, msgSignalValue);
//
//                if (++_cycle == 9000){
//                    break;
//                }
//            }
//        }


        void Generator::OnOneShotTimer(const std::string &timerid) {

        }



        Generator::~Generator() {
            libsoc_pwm_free(_pwm_output);
        }
    }
}

riaps::ComponentBase* create_component(_component_conf_j& config, riaps::Actor& actor){
    auto result = new timertest::components::Generator(config, actor);
    return result;
}

void destroy_component(riaps::ComponentBase* comp){
    delete comp;
}
