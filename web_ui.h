
#pragma once

#include <vector>
#include <utility>
#include <string>
#include <thread>

class WebUI{
  public:

    ~WebUI();

    /// run the web server in a separate thread
    int run( int argc, char** argv );
  private:

};
