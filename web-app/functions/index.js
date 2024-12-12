// const express = require('express');
// const serverless = require('serverless-http');

// const app = express();
// const router = express.Router();
// let currentColor = 'RED';


// app.get('/color', (req, res) => {
//     res.json({ color: currentColor });
// });

// app.post('/color', express.json(), (req, res) => {
//     if (req.body.color) {
//         currentColor = req.body.color;
//         console.log(req.body.color, currentColor);
//         res.json({ success: true, color: currentColor });
//     } else {
//         res.status(400).json({ success: false, message: 'Invalid color' });
//     }
// });

// app.use('/.netlify/functions/index', router);

// module.exports.handler = serverless(app);

const fs = require("fs");
const path = "/tmp/currentColor.txt";
const taskPath = "/tmp/currentTask.txt";

exports.handler = async (event) => {
  if (event.httpMethod === "GET") {
    // Read the color from the file or use the default
    let currentColor = "YELLOW"; // Default color if the file doesn't exist
    let currentTask = "Working on emb sys";
    if (fs.existsSync(path)) {
      currentColor = fs.readFileSync(path, "utf8");
    }

    if (fs.existsSync(taskPath)){
        currentTask = fs.readFileSync(taskPath, "utf8");
    }
    return {
      statusCode: 200,
      body: JSON.stringify({ task: currentTask, color: currentColor }),
    };
  }

  if (event.httpMethod === "PUT") {
    try {
      const body = JSON.parse(event.body); // Parse incoming JSON body
      if (body.color) {
        fs.writeFileSync(path, body.color, "utf8"); // Save the color to a file
      }

      if (body.task){
        fs.writeFileSync(taskPath, body.task, "utf8"); // Save the task to a file
      }

      return {
        statusCode: 200,
        body: JSON.stringify({task: body.task, color: body.color }),
      };
    //   return { statusCode: 400, body: JSON.stringify({ error: "Color not provided" }) };
    } catch (err) {
      return { statusCode: 400, body: JSON.stringify({ error: "Invalid JSON body" }) };
    }
  }

  return { statusCode: 405, body: JSON.stringify({ error: "Method not allowed" }) };
};
