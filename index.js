const express = require("express");
const app = express();
const path = require("path");
const formidable = require("formidable");
const ejs = require("ejs");
const readline = require("readline");
const fs = require("fs");
const exec = require("child_process").exec;

const router = express.Router();

const Level = require("./classes/Level.js");
const FileReader = require("./classes/FileReader.js");

app.set("view engine", "ejs");

app.get("/", function(req, res) {
  res.render("index");
});

app.get("/importFile", function(req, res) {
  res.render("solve", { fileName: "", levelCount: "" });
});

app.get("/playSokoban", function(req, res) {
  res.render("playSokoban");
});

app.get("/", function(req, res) {
  res.redirect("/");
});

app.post("/playLevel", function(req, res) {
  new formidable.IncomingForm().parse(req, (err, fields, files) => {
    if (err) {
      console.error("Error", err);
      throw err;
    }
    currentIndex = 1;
    fileName = files.fileToUpload.name;
    levels = new FileReader(files.fileToUpload.path);
    console.log(levels.levelsArray[0]);
    res.render("playSokobanLevel", {
      fileName,
      levelCount: levels.levelsArray.length,
      currentlySelected: currentIndex,
      level: levels.levelsArray[0]
    });
  });
});

app.get("/playLevel", function(req, res) {
  res.render("playSokobanLevel", {
    fileName,
    levelCount: levels.levelsArray.length,
    currentlySelected: currentIndex,
    level: levels.levelsArray[currentIndex - 1]
  });
});

app.post("/playOtherLevel", function(req, res) {
  new formidable.IncomingForm().parse(req, (err, fields, files) => {
    if (err) {
      console.error("Error", err);
      throw err;
    }
    currentIndex = fields.selectedLevel;
    res.render("playSokobanLevel", {
      fileName,
      levelCount: levels.levelsArray.length,
      currentlySelected: currentIndex,
      level: levels.levelsArray[currentIndex - 1]
    });
  });
});

app.get("/playOtherLevel", function(req, res) {
  res.render("playSokobanLevel", {
    fileName,
    levelCount: levels.levelsArray.length,
    currentlySelected: currentIndex,
    level: levels.levelsArray[currentIndex - 1]
  });
});

let solutions;
let sortingDesc = false;
app.get("/viewSolutions", function(req, res) {
  solutions = JSON.parse(fs.readFileSync("solutions/log.json"));
  if (req.query.sortingMethod) {
    switch (req.query.sortingMethod) {
      case "id":
        solutions.sort(
          (a, b) => parseInt(a.levelIndex) - parseInt(b.levelIndex)
        );
        break;
      case "name":
        solutions.sort((a, b) => a.fileName.localeCompare(b.fileName));
        break;
      case "method":
        solutions.sort((a, b) => a.method.localeCompare(b.method));
        break;
      case "time":
        solutions.sort(
          (a, b) => parseInt(a.milisecondsTaken) - parseInt(b.milisecondsTaken)
        );
        break;
      case "move":
        solutions.sort(
          (a, b) => parseInt(a.solution.length) - parseInt(b.solution.length)
        );
        break;
      case "push":
        solutions.sort(
          (a, b) =>
            parseInt(a.solution.match(/[A-Z]/g).length) -
            parseInt(b.solution.match(/[A-Z]/g).length)
        );
        break;
      case "date":
        solutions.sort(
          (a, b) => Date.parse(a.startDate) - Date.parse(b.startDate)
        );
        break;
      case "solved":
        solutions.sort(
          (a, b) => Boolean(a.solution.length) - Boolean(b.solution.length)
        );
        break;
    }
    if (sortingDesc) {
      sortingDesc = false;
    } else {
      sortingDesc = true;
      solutions.reverse();
    }
  }

  res.render("solutions", { solutions });
});

app.get("/details", function(req, res) {
  res.render("details", { solution: solutions[req.query.id] });
});

app.post("/solve", function(req, res) {
  new formidable.IncomingForm().parse(req, (err, fields, files) => {
    if (err) {
      console.error("Error", err);
      throw err;
    }
    doingTheSolvingWork(fields.method, fields.methodSub, fields.methodTimeout);
    res.render("scheduledToSolve", {
      currentlySelected: currentIndex
    });
  });
});

app.get(["/root", "/solve", "/upload", "/select"], function(req, res) {
  res.redirect("/");
});

let currentIndex;
let levels = [];
let fileName;
// Access the parse results as request.body
app.post("/upload", function(req, res) {
  new formidable.IncomingForm().parse(req, (err, fields, files) => {
    if (err) {
      console.error("Error", err);
      throw err;
    }
    currentIndex = 1;
    fileName = files.fileToUpload.name;
    levels = new FileReader(files.fileToUpload.path);
    console.log(levels.levelsArray[0]);
    res.render("solve", {
      fileName,
      levelCount: levels.levelsArray.length,
      currentlySelected: currentIndex,
      level: levels.levelsArray[0]
    });
  });
});

app.post("/select", function(req, res) {
  new formidable.IncomingForm().parse(req, (err, fields, files) => {
    if (err) {
      console.error("Error", err);
      throw err;
    }
    currentIndex = fields.selectedLevel;
    res.render("solve", {
      fileName,
      levelCount: levels.levelsArray.length,
      currentlySelected: currentIndex,
      level: levels.levelsArray[currentIndex - 1]
    });
  });
});

// Parse URL-encoded bodies (as sent by HTML forms)
app.use(express.urlencoded());
app.use(express.static(__dirname + "/"));

// Parse JSON bodies (as sent by API clients)
app.use(express.json());

const doingTheSolvingWork = async (method, methodSub, timeout) => {
  const levelString = levels.levelsArray[currentIndex - 1].board
    .join("")
    .match(
      new RegExp(".{1," + levels.levelsArray[currentIndex - 1].width + "}", "g")
    )
    .join("|");
  let execString;
  switch (method) {
    case "JSoko":
      execString =
        'java -jar solvers\\JSokoSolver.jar "' +
        levelString +
        '" ' +
        methodSub +
        " " +
        timeout * 1000;
      break;
    case "Kacper":
      execString =
        'solvers\\sokosolver.exe "' + levelString + '" ' + timeout * 1000;
      break;
  }
  console.log(execString);
  const fileJSON = JSON.parse(fs.readFileSync("solutions/log.json"));
  let time = new Date();
  exec(execString, function callback(error, stdout, stderr) {
    let timeFinal = Date.now() - time;
    let solution = {
      fileName,
      levelIndex: currentIndex,
      level: levels.levelsArray[currentIndex - 1],
      method: method,
      arguments: method != "Kacper" && methodSub,
      solution: stdout.trim(),
      milisecondsTaken: timeFinal,
      startDate: time.toLocaleString("en-GB", { timeZone: "UTC" })
    };

    fileJSON.push(solution);
    let data = JSON.stringify(fileJSON, null, 2);
    fs.writeFile("solutions/log.json", data, function(err) {
      if (err) throw err;
      console.log("Solution stored in the database!");
    });
    return;
  });
};

//add the router
app.use("/", router);
app.listen(process.env.port || 3000);

console.log("Running at Port 3000");
