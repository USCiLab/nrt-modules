// ######################################################################
// Get the appropriate websocket URL to the server that's serving this page
function this_websocket_url()
{
  var pcol;
  var u = document.URL;

  pcol = "ws://";
  u = u.substr(7);

  u = u.split('/');

  return pcol + u[0];
}

// ######################################################################
// Create a new websockets connection to an NRT server
//   
//  - nrt_protocol.start():
//       Start up the connection. Call this method after all callbacks have been registered
//   
//  - nrt_protocol.register_callback(MessageName, Callback):
//       Register a callback for a particular kind of message
//  
//  - nrt_protocol.request_federation_summary():
//       Request a new BlackboardFederationSummary to be sent out
//
function nrt_protocol(socket_address, on_open_callback, on_close_callback)
{
  this.socket_address = socket_address;

  this.on_open_callback = function() {}; 
  if(arguments.length >= 2)
    this.on_open_callback = on_open_callback;

  this.on_close_callback = function() {}; 
  if(arguments.length >= 3)
    this.on_close_callback = on_close_callback;

  this.callbacks = {};
  this.buffer    = "";
  this.is_open   = false;
}

// ######################################################################
nrt_protocol.prototype.start = function()
{
  var self = this;
  this.socket = new WebSocket(this.socket_address, "nrt-ws-protocol");
  this.socket.onopen  = function() { self.is_open = true; self.request_federation_summary(); self.on_open_callback();}
  this.socket.onclose = function() { self.is_open = false; self.on_close_callback(); }
  this.socket.onmessage = function(message_string){self.got_packet(message_string);};
}

// ######################################################################
nrt_protocol.prototype.handle_message = function(message_string)
{
  var message_object = JSON.parse(message_string);
  if(this.callbacks[message_object.msgtype] !== undefined)
    this.callbacks[message_object.msgtype](message_object.message);
}

// ######################################################################
nrt_protocol.prototype.got_packet = function(msg)
{ 
  this.buffer += msg.data;
  if(this.buffer.indexOf("NRT_MESSAGE_BEGIN") != 0)
  {
    var msgbegin = msg.data.indexOf("NRT_MESSAGE_BEGIN");
    if(msgbegin >= 0)
    {
      this.buffer = msg.data.substring(msgbegin);
    }
  }
  else
  {
    var msgend = this.buffer.indexOf("NRT_MESSAGE_END");
    if(msgend >= 0)
    {
      this.handle_message(this.buffer.substring("NRT_MESSAGE_BEGIN".length+1, msgend));
      this.buffer = this.buffer.substring(msgend);
    }
  }
}

// ######################################################################
nrt_protocol.prototype.register_callback = function(msgtype, callback)
{
  this.callbacks[msgtype] = callback;
}

// ######################################################################
nrt_protocol.prototype.request_federation_summary = function()
{
  var request = new Object();
  request.msgtype = "BlackboardFederationSummaryRequest";
  request.message = {};
  this.socket.send(JSON.stringify(request));
}
