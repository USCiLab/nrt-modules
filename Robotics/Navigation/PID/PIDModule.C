#include "PIDModule.H"

// ######################################################################
PIDModule::PIDModule( std::string const & instanceName ) :
  nrt::Module( instanceName ),
  itsPidComponent( instanceName )
{
  //
}

// ######################################################################
void PIDModule::onMessage( DesiredValuePort port )
{
  itsPidComponent.setDesiredValue( port->value );

  itsPidComponent.update();
  //post<OutputValuePort>( nrt::make_unique( new nrt::Message<double>( itsPidComponent.update() ) ) );
}

// ######################################################################
void PIDModule::onMessage( ObservedValuePort port )
{
  itsPidComponent.setObservedValue( port->value );

  auto output_msg = nrt::make_unique( new nrt::Message<double>( itsPidComponent.update() ) );
  post<OutputValuePort>( output_msg );
}

NRT_REGISTER_MODULE(PIDModule);
