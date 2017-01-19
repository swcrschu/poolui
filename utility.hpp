#include <Wt/WApplication>
#include <thread>

// use for update_lock triggerUpdate (Button)
inline void fire_and_forget(std::function<void()> f, Wt::WApplication* app) {
  std::thread([=]() {
    // getting update lock TIMER implementieren
    auto update_lock = app->getUpdateLock();
    f();
    app->triggerUpdate();
  }).detach();
}


