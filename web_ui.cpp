/*
 * Copyright (C) 2008 Emweb bvba, Heverlee, Belgium.
 *
 * See the LICENSE file for terms of use.
 */


#include "web_ui.h"
#include <map>
#include <Wt/WBootstrapTheme>
#include <Wt/WMenu>
#include <Wt/WStackedWidget>
#include <Wt/WMessageBox>
#include <Wt/WNavigationBar>
#include <Wt/WPopupMenu>
#include <Wt/WPopupMenuItem>
#include <Wt/WContainerWidget>
#include <Wt/WAnchor>
#include <Wt/WLineEdit>
#include <Wt/WLink>
#include <Wt/WText>
#include <Wt/WTextArea>
#include <Wt/WPushButton>
#include <Wt/WTemplate>
#include <Wt/WApplication>
#include <Wt/WServer>
#include <Wt/WHBoxLayout>
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

  //define class

  WLineEdit *nameEdit_;
  WTextArea *command_output;
  std::map<string,string> key_to_command;	    // key value map command file key_to_command

 void GetCommandoutput( std::string, WTextArea*);   // read command output line
 void read_input_file(std::string filename);	    // 
};

WebUIApplication::WebUIApplication(const WEnvironment& env)
  : WApplication(env)
{
  setTitle("poolui");                               // application title

// css
  Wt::WBootstrapTheme *bootstrapTheme = new Wt::WBootstrapTheme(this);
  bootstrapTheme->setVersion(Wt::WBootstrapTheme::Version3);
  bootstrapTheme->setResponsive(true);
  setTheme(bootstrapTheme);
  useStyleSheet("resources/themes/bootstrap/3/bootstrap-theme.min.css"); 

  // needed to load the templates for the objects and make the bootstrap theme work
  this->messageResourceBundle().use(this->appRoot() + "text");

  // Config file read
   read_input_file("key_to_command");
  // menuContainer + menu
  Wt::WContainerWidget *menuContainer = new Wt::WContainerWidget();
    root()->addWidget(menuContainer);

  Wt::WNavigationBar *navigation = new Wt::WNavigationBar(menuContainer);
     navigation->setTitle("Link","http://www.google.com/search?q=corpy+inc");
      navigation->setResponsive(true); 
      
      // output WText menuItem
     Wt::WStackedWidget *contentsStack = new Wt::WStackedWidget(menuContainer);
      contentsStack->addStyleClass("contents");

      // Setup a Left-aligned	  
      Wt::WMenu *leftMenu = new Wt::WMenu(contentsStack, menuContainer);
      navigation->addMenu(leftMenu);

          leftMenu->addItem("Home",new Wt::WText("alternative Text"));
	  leftMenu->addItem("link2", new Wt::WText("link2"));      
          leftMenu->addItem("link3", new Wt::WText("link3"));
 
  // contentContainer + content
   Wt::WContainerWidget *contenContainer = new Wt::WContainerWidget();
     root()->addWidget(contenContainer);

    //button 
	  Wt::WPushButton *pb1 = new Wt::WPushButton("Button1" );
	  root()->addWidget(pb1);
	  
	  Wt::WPushButton *pb2 = new Wt::WPushButton("Button2" );
	  root()->addWidget(pb2);


          command_output = new WTextArea();
          root()->addWidget( command_output );


   //action button 
   pb1->clicked().connect(std::bind([=](){
	GetCommandoutput(key_to_command["date"] , command_output );
       }
    ));
   pb2->clicked().connect(std::bind([=](){
	GetCommandoutput( "ls -ltr", command_output );
       }
    ));


}

  // CommandLine to TextAarea
void WebUIApplication::GetCommandoutput( std::string command, WTextArea* text_area){
  std::string text; 
  FILE *in;
  char buff[512];
   if(!(in = popen(command.c_str(), "r"))){
       return ;
     }
     while(fgets(buff, sizeof(buff), in)!=NULL){
 
         std::string line(&buff[0]);
    text+= line;

	 //	cout << buff ;  
       }
     pclose(in);
      text_area->setText(text); 
      return ;
  }
   

  WApplication *createApplication(const WEnvironment& env)
{
  return new WebUIApplication(env);
}

void WebUIApplication::read_input_file( std::string filename ) {
  ifstream in(filename);
  assert( in.good() );
  
  std::string line;
  while( getline( in, line ) ){
    stringstream line_stream( line );
    std::string key, value_part;
    std::string combined_value;
    line_stream >> key ;
    if (key[0] == '#' || key == ""){
      continue;
    }
    while ( line_stream >> value_part ) {
      combined_value += value_part + " ";
    }
    key_to_command[key] = combined_value;
  }
  for( auto&& element : key_to_command ){
    cout << element.first << " " << element.second << endl;
      
  }
  
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
