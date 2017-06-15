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


// global function filter \r
inline std::string filter_character(std::string str) {
//  cout << __PRETTY_FUNCTION__ << endl;
  std::replace_if(begin(str), end(str), [](auto c) { return c == '\r'; }, ' ');
//  cout << "done " << __PRETTY_FUNCTION__ << endl;
  return str;
}

inline std::string filter_newline( std::string str ) {
    std::string new_string;
    for( char c : str ){
      if ( !((int)c == 13 || (int)c == 10 ) ){
	new_string += c;
      }
    }
    return new_string;
}


