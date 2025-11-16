//
// Created by Saar.Nagar on 09/11/2025.
//

#ifndef BUTTON_SHIFTER_TIMER_H
#define BUTTON_SHIFTER_TIMER_H
#include "xx_share_config.h"

namespace shifter
{

typedef enum class _timer_state
  : uint8_t
{
  timer_not_set = 0,
  timer_is_set,
  timer_running,
  timer_paused,
  timer_stop,
  timer_ended
} mx_timer_state;

class Timer
{
private:

public:
  mx_timer_state timer_state{ mx_timer_state::timer_not_set };
  float          secondsToRun{0.0f};
  float          osSecondsPassed{ -1.0f }; // store osClock delta
  double         cumulativeOsTime_sec{ -1.0 };

  shifter::strct::time_fragment_def tf_begin;

  Timer& operator=(Timer const& inTimer) = default;

  void reset()
  {
    osSecondsPassed      = 0.0f;
    timer_state          = mx_timer_state::timer_not_set;
    cumulativeOsTime_sec = 0.0f;
    secondsToRun         = 0.0f;
  }

  void stop()
  {
    timer_state = mx_timer_state::timer_stop;
  }

  // ----------------------------------------------------

  void pause()
  {
    timer_state = mx_timer_state::timer_paused;
  }

  [[nodiscard]] double getCumulativeOSTimeInSec() const { return this->cumulativeOsTime_sec; }

  void setEnd() { timer_state = mx_timer_state::timer_ended; }

  void resume()
  {
    timer_state = mx_timer_state::timer_running;
  }

  [[nodiscard]] float getOsSecondsPassed() const { return osSecondsPassed; }

  float getSecondsPassed(bool bCalcOsDelta = false)
  {
   return this->getOsSecondsPassed();
  }

  [[nodiscard]] bool isRunning() const { return (timer_state == mx_timer_state::timer_running) ? true : false; }

  mx_timer_state getState() { return timer_state; }

  // initialize class. There is no default contractor
  static void init(shifter::strct::time_fragment_def & outTime)
  {
    outTime.reset (); // reset function, also initializes the time
  }

  static void start(Timer& inTimer, float inSecondsToRun = 0.0f)
  {
    Timer::init (inTimer.tf_begin); // init() function also reset + store the current time
    inTimer.timer_state = mx_timer_state::timer_running;
    inTimer.secondsToRun = inSecondsToRun;
  }

  static float get_os_duration_between2_time_fragments(shifter::strct::time_fragment_def& t2, shifter::strct::time_fragment_def& t1)
  {
    const auto duration = std::chrono::duration<float>(t2.os_clock - t1.os_clock);
    return duration.count();
  }

  static float get_os_delta_time_now_vs_start_in_milli(const shifter::strct::time_fragment_def& inStartTimeFragment)
  {
    const std::chrono::time_point<std::chrono::steady_clock> os_clock = std::chrono::steady_clock::now(); // fetch NOW
    const std::chrono::duration<float, std::milli> duration = os_clock - inStartTimeFragment.os_clock;
    return duration.count (); // milliseconds
  }

  static bool wasEnded(shifter::Timer& inTimer)
  {
    if (inTimer.secondsToRun <= 0.0f)
      return false;

    auto delta_since_start_tf_in_sec = Timer::get_os_delta_time_now_vs_start_in_milli (inTimer.tf_begin) / 1000.0f;
    if ( inTimer.secondsToRun < delta_since_start_tf_in_sec )
      return false;

    return true;
  }
};

} // shifter

#endif // BUTTON_SHIFTER_TIMER_H
