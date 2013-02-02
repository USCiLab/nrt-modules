#include <nrt/Core/Blackboard/details/BlackboardHelpers.H>
#include "libs/rapidjson/prettywriter.h"
#include "libs/rapidjson/writer.h"
#include "libs/rapidjson/stringbuffer.h"
#include <sstream>

typedef rapidjson::Writer<rapidjson::StringBuffer> jsonwriter;

// ######################################################################
void toJSON(nrt::ParameterSummary const & ps, jsonwriter & writer)
{
  writer.StartObject();

  writer.String("state");
  switch(ps.state)
  {
    case nrt::ParameterState::Create:  writer.String("Create");  break;
    case nrt::ParameterState::Modify:  writer.String("Modify");  break;
    case nrt::ParameterState::Read:    writer.String("Read");    break;
    case nrt::ParameterState::Destroy: writer.String("Destroy"); break;
  }

  writer.String("descriptor"); writer.String(ps.descriptor.c_str());

  writer.String("name"); writer.String(ps.name.c_str());

  writer.String("description"); writer.String(ps.description.c_str());

  writer.String("valuetype"); writer.String(ps.valuetype.c_str());

  writer.String("defaultvalue"); writer.String(ps.defaultvalue.c_str());

  writer.String("value"); writer.String(ps.value.c_str());

  writer.String("validvalues"); writer.String(ps.validvalues.c_str());

  writer.String("category"); writer.String(ps.categorydescription.c_str());

  writer.EndObject();
}

// ######################################################################
void toJSON(nrt::blackboard::ModulePortSummary const & mps, jsonwriter & writer)
{
  writer.StartObject();

  writer.String("msgtype"); writer.String(mps.msgtype.c_str());

  writer.String("rettype"); writer.String(mps.rettype.c_str());

  writer.String("portname"); writer.String(mps.portname.c_str());

  writer.String("description"); writer.String(mps.description.c_str());

  writer.String("topi"); writer.String(mps.topi.c_str());

  writer.EndObject();
}

// ######################################################################
void toJSON(std::string const & bbuid, std::string const & moduid, nrt::blackboard::ModuleSummary const & ms, jsonwriter & writer)
{
  writer.StartObject();

  writer.String("bbuid"); writer.String(bbuid.c_str());

  writer.String("moduid"); writer.String(moduid.c_str());

  writer.String("classname"); writer.String(ms.classname.c_str());

  writer.String("instance"); writer.String(ms.instance.c_str());

  writer.String("parent"); writer.String(ms.parent.c_str());

  writer.String("posters");
  {
    writer.StartArray();
    for(nrt::blackboard::ModulePortSummary const & mps : ms.posters)
      toJSON(mps, writer);
    writer.EndArray();
  }

  writer.String("checkers");
  {
    writer.StartArray();
    for(nrt::blackboard::ModulePortSummary const & mps : ms.checkers)
      toJSON(mps, writer);
    writer.EndArray();
  }

  writer.String("subscribers");
  {
    writer.StartArray();
    for(nrt::blackboard::ModulePortSummary const & mps : ms.subscribers)
      toJSON(mps, writer);
    writer.EndArray();
  }

  writer.String("parameters");
  {
    writer.StartArray();
    for(std::pair<std::string, nrt::ParameterSummary> const & p : ms.parameters)
    {
      nrt::ParameterSummary const & ps = p.second;
      toJSON(ps, writer);
    }
    writer.EndArray();
  }

  writer.EndObject();
}

// ######################################################################
void toJSON(std::string const & namespc, std::string const & connec, nrt::blackboard::ConnectorSummary const & cs, jsonwriter & writer)
{
  writer.StartObject();

  writer.String("namespc"); writer.String(namespc.c_str());

  writer.String("connec"); writer.String(connec.c_str());

  writer.String("msgtype"); writer.String(cs.msgtype.c_str());

  writer.String("rettype"); writer.String(cs.rettype.c_str());

  writer.String("instance"); writer.String(cs.instance.c_str());

  writer.String("type"); 
  switch(cs.type)
  {
    case nrt::ConnectorType::Intra:  writer.String("Intra"); break;
    case nrt::ConnectorType::Border: writer.String("Border"); break;
  }

  writer.String("topic"); writer.String(cs.topic.c_str());

  writer.String("topicfilt"); writer.String(cs.topicfilt.c_str());

  writer.EndObject();
}


// ######################################################################
void toJSON(nrt::blackboard::ConnectionSummary const & cs, jsonwriter & writer)
{
  writer.StartObject();

  writer.String("bbuid1");    writer.String(cs.bbuid1.c_str());     

  writer.String("module1");   writer.String(cs.module1.c_str());    

  writer.String("portname1"); writer.String(cs.portname1.c_str());  

  writer.String("bbuid2");    writer.String(cs.bbuid2.c_str());     

  writer.String("module2");   writer.String(cs.module2.c_str());    

  writer.String("portname2"); writer.String(cs.portname2.c_str());  

  writer.EndObject();
}

// ######################################################################
void toJSON(std::string const & namespacename, nrt::blackboard::NamespaceSummary const & nss, jsonwriter & writer)
{
  writer.StartObject();

  writer.String("name"); writer.String(namespacename.c_str());

  writer.String("modules");
  {
    writer.StartArray();
    for(std::pair<std::string, std::map<std::string, nrt::blackboard::ModuleSummary>> const & p : nss.modules)
    {
      std::string const & bbuid = p.first;
      for(std::pair<std::string, nrt::blackboard::ModuleSummary> const & q : p.second)
      {
        std::string const & moduid = q.first;
        nrt::blackboard::ModuleSummary const & ms = q.second;
        toJSON(bbuid, moduid, ms, writer);
      }
    }
    writer.EndArray();
  }

  writer.String("pconnectors");
  {
    writer.StartArray();
    for(std::pair<std::string, std::map<std::string, nrt::blackboard::ConnectorSummary>> const & p : nss.pconnectors)
    {
      std::string const & namespc = p.first;
      for(std::pair<std::string, nrt::blackboard::ConnectorSummary> const & q : p.second)
      {
        std::string const & connec = q.first;
        nrt::blackboard::ConnectorSummary const & cs = q.second;
        toJSON(namespc, connec, cs, writer);
      }
    }
    writer.EndArray();
  }

  writer.String("cconnectors");
  {
    writer.StartArray();
    for(std::pair<std::string, std::map<std::string, nrt::blackboard::ConnectorSummary>> const & p : nss.cconnectors)
    {
      std::string const & namespc = p.first;
      for(std::pair<std::string, nrt::blackboard::ConnectorSummary> const & q : p.second)
      {
        std::string const & connec = q.first;
        nrt::blackboard::ConnectorSummary const & cs = q.second;
        toJSON(namespc, connec, cs, writer);
      }
    }
    writer.EndArray();
  }

  writer.String("sconnectors");
  {
    writer.StartArray();
    for(std::pair<std::string, std::map<std::string, nrt::blackboard::ConnectorSummary>> const & p : nss.sconnectors)
    {
      std::string const & namespc = p.first;
      for(std::pair<std::string, nrt::blackboard::ConnectorSummary> const & q : p.second)
      {
        std::string const & connec = q.first;
        nrt::blackboard::ConnectorSummary const & cs = q.second;
        toJSON(namespc, connec, cs, writer);
      }
    }
    writer.EndArray();
  }

  writer.String("connections");
  {
    writer.StartArray();
    for(nrt::blackboard::ConnectionSummary const & cs : nss.connections)
    {
      toJSON(cs, writer);
    }
    writer.EndArray();
  }

  writer.String("subnamespaces");
  {
    writer.StartArray();
    for(std::pair<std::string, std::shared_ptr<nrt::blackboard::NamespaceSummary>> const & p : nss.subnamespaces)
    {
      std::string const & relativenamespc = p.first;
      nrt::blackboard::NamespaceSummary const & nss = *p.second;
      toJSON(relativenamespc, nss, writer);
    }
    writer.EndArray();
  }

  writer.EndObject();
}

// ######################################################################
std::string toJSON(nrt::blackboard::BlackboardFederationSummary const & summary)
{

  rapidjson::StringBuffer buffer;

  jsonwriter writer(buffer);

  {
    writer.StartObject(); 

    writer.String("msgtype");
    writer.String("BlackboardFederationSummary");

    writer.String("message");
    {
      writer.StartObject();

      // Write the bbnicks
      writer.String("bbnicks");
      {
        writer.StartArray();
        for(auto p : summary.bbnicks)
        {
          writer.StartObject();
          writer.String("uid"); writer.String(p.first.c_str()); 
          writer.String("nick"); writer.String(p.second.c_str());
          writer.EndObject();
        }
        writer.EndArray();
      }

      writer.String("namespaces");
      {
        writer.StartArray();
        toJSON("/", *summary.nss, writer);
        writer.EndArray();
      }

      writer.EndObject();
    }

    writer.EndObject();
  }

  return buffer.GetString();
}

// ######################################################################
std::string toJSON(nrt::ModuleParamChangedMessage const & message)
{
  rapidjson::StringBuffer buffer;
  jsonwriter writer(buffer);

  {
    writer.StartObject();

    writer.String("msgtype");
    writer.String("ModuleParamChangedMessage");

    writer.String("message");
    {
      writer.StartObject();

      writer.String("moduleuid");
      writer.String(message.moduleuid.c_str());

      writer.String("paramsummary");
      toJSON(message.paramsummary, writer);

      writer.EndObject();
    }

    writer.EndObject();
  }
  return buffer.GetString();
}


// ######################################################################
void toJSON(nrt::blackboard::GUIdata const & guidata, jsonwriter & writer)
{
  writer.String("x"); writer.Int(guidata.x);
  writer.String("y"); writer.Int(guidata.y);
}

// ######################################################################
std::string toJSON(nrt::GUIdataMessage const & message)
{
  rapidjson::StringBuffer buffer;
  jsonwriter writer(buffer);

  {
    writer.StartObject();

    writer.String("msgtype"); writer.String("GUIdataMessage");

    writer.String("message");
    {
      writer.StartArray();
      for(std::pair<std::string, nrt::blackboard::GUIdata> const & guidata : message.guidata)
      {
        writer.StartObject();
        writer.String("id"); writer.String(guidata.first.c_str());
        toJSON(guidata.second, writer);
        writer.EndObject();
      }
      writer.EndArray();
    }

    writer.EndObject();
  }
  return buffer.GetString();
}


// ######################################################################
std::string toJSON(nrt::LoaderSummaryMessage const & message)
{
  return "{}";
  // rapidjson::StringBuffer buffer;
  // jsonwriter writer(buffer);
  // {
  //   writer.StartObject();
  // 
  //   writer.String("msgtype"); writer.String("LoaderSummaryMessage");
  // 
  //   writer.String("message");
  //   {
  //     writer.StartArray();
  //     for(std::pair<std::string, nrt::blackboard::GUIdata> const & guidata : message.guidata)
  //     {
  //       writer.StartObject();
  //       writer.String("id"); writer.String(guidata.first.c_str());
  //       toJSON(guidata.second, writer);
  //       writer.EndObject();
  //     }
  //     writer.EndArray();
  //   }
  // 
  //   writer.EndObject();
  // }
  // return buffer.GetString();
}


