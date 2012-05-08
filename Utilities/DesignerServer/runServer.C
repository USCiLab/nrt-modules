#include "DesignerServerModule.H"
#include <nrt/Core/Model/Manager.H>
#include <nrt/Core/Util/DynamicLoader.H>

std::string exec(std::string cmd) 
{
  FILE* pipe = popen(cmd.c_str(), "r");
  if (!pipe) throw std::runtime_error("Could not open command");
  char buffer[128];
  std::string result = "";
  while(!feof(pipe)) {
    if(fgets(buffer, 128, pipe) != NULL)
      result += buffer;
  }
  pclose(pipe);
  return result;
}


int main(int argc, const char * argv[])
{
  nrt::Manager mgr(argc, argv);

  std::string filename = "./DesignerServerModule.so";
  std::string command = "nm " + filename + " | grep _createModule | cut -f 3 -d\\ | tr -d '\n'";
  std::string functionName = exec(command);
  std::cout << "Function Name [" << functionName << "]" << std::endl;

  nrt::DynamicLoader loader(filename);
  auto creationFunction = loader.load<std::shared_ptr<nrt::Module>(std::string const &)>(functionName);

  std::shared_ptr<nrt::Module> module = creationFunction("DesignerServer");
  mgr.addSubComponent(module);

  mgr.launch();
  mgr.wait();

  return 0;
}
