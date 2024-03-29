// const express = require('express');
// const http = require('http');
// const WebSocket = require('ws');
// const path = require('path');

// const app = express();

// // Serve static files from the 'public' directory
// app.use(express.static(path.join(__dirname, 'public')));

// // Create an HTTP server
// const server = http.createServer(app);

// // Create a WebSocket server
// const wss = new WebSocket.Server({ server });

// // WebSocket connection handler
// wss.on('connection', function connection(ws) {
//   console.log('WebSocket connection established');

//   // Event handler for receiving messages from Arduino
//   ws.on('message', function incoming(message) {
//     // Broadcast the received message to all clients (including the web page)
//     wss.clients.forEach(function each(client) {
//       if (client !== ws && client.readyState === WebSocket.OPEN) {
//         client.send(message);
//       }
//     });
//   });
// });

// const PORT = process.env.PORT || 3000;

// // Start the server
// server.listen(PORT, () => {
//   console.log(`Server running on port ${PORT}`);
// });





const express = require('express');
const bodyParser = require('body-parser');
const path = require('path');

const app = express();
const port = 3000;

app.use(bodyParser.json());

let jsonData = null; // Variable to store the received JSON data

// Define a route handler for the root endpoint
app.get('/', (req, res) => {
  res.send('Welcome to your server!!!');
});

// Define a route handler for GET requests to '/json_endpoint'
app.get('/json_endpoint', (req, res) => {
  if (jsonData !== null) {
    res.send('GET request received at /json_endpoint\n' + JSON.stringify(jsonData));
  } else {
    res.send('No JSON data has been received yet');
  }
});

// Define a route handler for POST requests to '/json_endpoint'
app.post('/json_endpoint', (req, res) => {
  // Handle incoming JSON data
  console.log('Received JSON data:', req.body);
  // Store the received JSON data
  jsonData = req.body;
});

// Serve the index.html file
app.get('/table', (req, res) => {
  res.sendFile(path.join(__dirname, '/public/index.html'));
});

app.listen(port, () => {
  console.log(`Server is running on http://localhost:${port}`);
});
