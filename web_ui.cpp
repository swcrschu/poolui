/*
 * Copyright (C) 2008 Emweb bvba, Heverlee, Belgium.
 *
 * See the LICENSE file for terms of use.
 */


#include "web_ui.h"
#include <Wt/WBootstrapTheme>
#include <Wt/WMenu>
#include <Wt/WStackedWidget>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WTemplate>
#include <Wt/WApplication>
#include <Wt/WServer>
#include <iostream>
#include <sstream>

//#include "ImagesWidget.h"

using namespace Wt;
using namespace std;
// c++0x only, for std::bind
// #include <functional>

class WebUIApplication : public WApplication
{
public:
  WebUIApplication(const WEnvironment& env);

private:
  WLineEdit *nameEdit_;
};


WebUIApplication::WebUIApplication(const WEnvironment& env)
  : WApplication(env)
{
  setTitle("poolui");                               // application title
  Wt::WBootstrapTheme *bootstrapTheme = new Wt::WBootstrapTheme(this);
  bootstrapTheme->setVersion(Wt::WBootstrapTheme::Version3);
  bootstrapTheme->setResponsive(true);
  setTheme(bootstrapTheme);
  useStyleSheet("resources/themes/bootstrap/3/bootstrap-theme.min.css");

  // needed to load the templates for the objects and make the bootstrap theme work
  this->messageResourceBundle().use(this->appRoot() + "text");
  
  Wt::WContainerWidget *main_container = new Wt::WContainerWidget();

  root()->addWidget( main_container );

  nameEdit_ = new WLineEdit();
  main_container->addWidget( nameEdit_ );

  Wt::WPushButton* pb = new Wt::WPushButton("Execute Action");
  main_container->addWidget( pb );

  pb->clicked().connect( std::bind( [=] () {
    FILE *in;
    char buff[512];
      
	if(!(in = popen("ls -sail", "r"))){
        return 1;
	  }
	    while(fgets(buff, sizeof(buff), in)!=NULL){
	        cout << buff;
		  }
		    pclose(in);
		    return 0;
	}
  ));


}

WApplication *createApplication(const WEnvironment& env)
{
  return new WebUIApplication(env);
}


int WebUI::run(int argc, char **argv)
{
  /*
   * The last argument to WRun specifies the function that will instantiate
   * new application objects. That function is executed when a new user surfs
   * to the Wt application, and after the library has negotiated browser
   * support. The function should return a newly instantiated application
   * object.
   */
  
  return WRun(argc, argv, &createApplication);
}

WebUI::~WebUI() {
  
}
