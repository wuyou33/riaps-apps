//
// Auto-generated by edu.vanderbilt.riaps.generator.ComponenetGenerator.xtend
//
#ifndef RIAPS_FW_TSCAGPIO_H
#define RIAPS_FW_TSCAGPIO_H

#include "TscaGpioBase.h"
#include <libsoc_debug.h>
#include <libsoc_gpio.h>

#define GROUP_TYPE_GROUPA "groupA"
#define PPS_OUTPUT 31

#define TOGGLE_ACTION_ID "ToggleGpio31"

namespace tsyncgpio {
  namespace components {

    class TscaGpio : public TscaGpioBase {

    public:
      TscaGpio(_component_conf &config, riaps::Actor &actor);

      virtual void OnClock(riaps::ports::PortBase *port);

      void OnGroupMessage(const riaps::groups::GroupId &groupId,
                          capnp::FlatArrayMessageReader &capnpreader,
                          riaps::ports::PortBase *port);
      void OnAnnounce(const riaps::groups::GroupId &groupId,
                      const std::string &proposeId, bool accepted);

      void OnActionPropose(riaps::groups::GroupId &groupId,
                           const std::string &proposeId,
                           const std::string &actionId,
                           const timespec &timePoint);

      virtual ~TscaGpio();

    private:
      bool m_hasJoined;

      void ActionA(const uint64_t timerId);

      std::random_device m_rd;
      std::mt19937 m_generator;
      std::uniform_int_distribution<int> m_distrPeriod;

      std::unordered_map<std::string, timespec> m_accepted;
      std::unordered_map<uint64_t, timespec> m_scheduled;
      std::set<std::string> m_pendingActions;
      gpio *m_pps_output;
    };
  } // namespace components
} // namespace tsyncgpio

extern "C" riaps::ComponentBase *create_component(_component_conf &,
                                                  riaps::Actor &actor);
extern "C" void destroy_component(riaps::ComponentBase *);

#endif // RIAPS_FW_TSCAGPIO_H
