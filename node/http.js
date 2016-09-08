//Lets require/import the HTTP module
var fs   = require('fs');
var http = require('http');
var dispatcher = require('httpdispatcher');
var qs = require('querystring');

const PORT = 8080; 

//Lets use our dispatcher
function handleRequest(request, response)
{
    try {
        //console.log(request.url);
        dispatcher.dispatch(request, response);
    } catch(err) {
        console.log(err);
    }
}

//For all your static (js/css/images/etc.) set the directory name (relative path).
dispatcher.setStatic('res');
dispatcher.setStaticDirname('static');

dispatcher.onGet("/", 
function(req, res) {
    
    fs.readFile('./res/index.html', 
    function(error, content) {
      res.writeHead(200, {'Content-Type': 'text/html'});
      res.end(content, 'utf-8');
    });
    
});

//A sample POST request
dispatcher.onPost("/post1", 
function(req, res) {
  
  var json = JSON.parse(req.body);
  /// now sort it
  var sorted = json.sort(
  function N(a, b) {
    return b._id < a._id ? 1
        :  b._id > a._id ? -1
        : 0;
  });
  ///
  res.writeHead(200, {'Content-Type': 'text/html'});
  res.end(JSON.stringify(sorted), 'utf-8');
  
});

//Create a server
var server = http.createServer(handleRequest);

//Lets start our server
server.listen(PORT, 
function() {
  //Callback triggered when server is successfully listening. Hurray!
  console.log("Server listening on %s", PORT);
});
