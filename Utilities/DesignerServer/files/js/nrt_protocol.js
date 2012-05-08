if (typeof String.prototype.startsWith != 'function') {
  String.prototype.startsWith = function (str){
    return this.indexOf(str) == 0;
  };
}

// Create a new websockets connection to an NRT server
function nrt_protocol(socket_address)
{
  var self = this;
  self.socket = new WebSocket(socket_address, "nrt-ws-protocol");

  self.is_open = false;
  self.socket.onopen  = function() { self.is_open = true; }
  self.socket.onclose = function() { self.is_open = false; }


  self.buffer = "";
  self.socket.onmessage = function got_packet(msg)
  {
    self.buffer += msg.data;
    if(!self.buffer.startsWith("NRT_MESSAGE_BEGIN"))
    {
      var msgbegin = msg.data.indexOf("NRT_MESSAGE_BEGIN");
      if(msgbegin >= 0)
      {
        self.buffer = msg.data.substring(msgbegin);
      }
    }
    else
    {
      var msgend = self.buffer.indexOf("NRT_MESSAGE_END");
      if(msgend >= 0)
      {
        self.handle_message(self.buffer.substring("NRT_MESSAGE_BEGIN".length+1, msgend));
        self.buffer = self.buffer.substring(msgend);
      }
    }





    //console.log("Got Message: " + msg.data);

    //if(msg.data == "NRT_MESSAGE_BEGIN")
    //{
    //  console.log("Message beginning");
    //  self.buffer = [];
    //}
    //else if(msg.data == "NRT_MESSAGE_END") 
    //{
    //  console.log("Message Ending");
    //  self.handle_message(self.buffer.join(""));
    //  self.buffer = [];
    //}
    //else
    //{
    //  self.buffer.push(msg.data);
    //}
  }
}

nrt_protocol.prototype.handle_message = function(message_string)
{
  console.log("Got Message: " + message_string);
  var message_object = JSON.parse(message_string);
}
