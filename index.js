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

const preinstalledLevelSets = require("./classes/PreinstalledLevelSets.js");

app.set("view engine", "ejs");

app.get("/", function(req, res) {
  res.render("index");
});

app.get("/credits", function(req, res) {
  res.render("credits");
});

let levelPakDesc = false;
const sortLevelPaks = (sortingMethod) => {
  switch (sortingMethod) {
    case "year":
      preinstalledLevelSets.sort(
        (a, b) => parseInt(a.year) - parseInt(b.year)
      );
      break;
    case "levelCount":
      preinstalledLevelSets.sort(
        (a, b) => parseInt(a.levelCount) - parseInt(b.levelCount)
      );
      break;
    case "levelSetName":
      preinstalledLevelSets.sort((a, b) => a.levelSetName.localeCompare(b.levelSetName));
      break;
    case "author":
      preinstalledLevelSets.sort((a, b) => a.author.localeCompare(b.author));
      break;
  }
  if (levelPakDesc) {
    levelPakDesc = false;
  } else {
    levelPakDesc = true;
    preinstalledLevelSets.reverse();
  }
}

app.get("/importFile", function(req, res) {
  if (req.query.sortingMethod) {
    sortLevelPaks(req.query.sortingMethod);
  }
  res.render("solve", { fileName: "", levelCount: "", preinstalledLevelSets, mode: "solvePreselec"});
});

app.get("/playSokoban", function(req, res) {
  if (req.query.sortingMethod) {
    sortLevelPaks(req.query.sortingMethod);
  }
  res.render("playSokoban", {preinstalledLevelSets, mode: "playPreselec"});
});

app.get("/manageLevelSets", function(req, res) {
  if (req.query.sortingMethod) {
    sortLevelPaks(req.query.sortingMethod);
  }
  res.render("manageLevelSets", {preinstalledLevelSets, mode: "detailsPreselec"});
});

app.get("/levelEditMenu", function(req, res) {
  let levels = [];
  fs.readdir('levelsets/imported', (err, files) => {
    files.forEach(file => {
      levels.push(file);
    });
    res.render("levelEditMenu", {levels});
  });
});

app.get("/createANewLevelSet", function(req, res) {
  levels = new FileReader("levelsets/defaultlevel.xsb");
  res.render("createANewLevelSet", {fileName: '', level: levels.levelsArray[0], index: 0});
});


app.get("/addALevel", function(req, res) {
  let levelSetLength = levels.levelsArray.length;
  levels = new FileReader("levelsets/defaultlevel.xsb");
  res.render("createANewLevelSet", {fileName, level: levels.levelsArray[0], index: levelSetLength+1});
});

app.get("/editALevel", function(req, res) {
  res.render("createANewLevelSet", {fileName, level: levels.levelsArray[currentIndex-1], index: currentIndex});
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
    res.render("playSokobanLevel", {
      fileName,
      levelCount: levels.levelsArray.length,
      currentlySelected: currentIndex,
      level: levels.levelsArray[0]
    });
  });
});

app.get("/playPreselec", function(req, res) {
  currentIndex = req.query.currentIndex || 1;
  fileName = req.query.fileSetName;
  if(req.query.custom) levels = new FileReader("levelsets/imported/" + fileName);
  else levels = new FileReader("levelsets/preinstalled/" + fileName);
  res.render("playSokobanLevel", {
    fileName,
    levelCount: levels.levelsArray.length,
    currentlySelected: currentIndex,
    level: levels.levelsArray[currentIndex-1]
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

app.get("/deleteTheSet", function(req, res) {
  fs.unlinkSync("levelsets/imported/" + fileName);
  res.redirect("/levelEditMenu");
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

app.get("/customLevelSetDetails", function(req, res) {
  currentIndex = req.query.levelNumber || 1;
  fileName = req.query.fileSetName || fileName;
  levels = new FileReader("levelsets/imported/" + req.query.fileSetName);
  res.render("customLevelSetDetails", {
    fileName,
    levelCount: levels.levelsArray.length,
    currentlySelected: currentIndex,
    level: levels.levelsArray[0]
  });
});

app.post("/customLevelSetDetailsLevel", function(req, res) {
  console.log(levels.levelsArray);
  new formidable.IncomingForm().parse(req, (err, fields, files) => {
    if (err) {
      console.error("Error", err);
      throw err;
    }
    currentIndex = fields.selectedLevel;
    res.render("customLevelSetDetails", {
      fileName,
      levelCount: levels.levelsArray.length,
      currentlySelected: currentIndex,
      level: levels.levelsArray[currentIndex-1]
    });
  });
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

app.post("/addCustomLevelSet", function(req, res) {
  new formidable.IncomingForm().parse(req, (err, fields, files) => {
    if (err) {
      console.error("Error", err);
      throw err;
    }
    levelIndex = fields.index;
    let update = false;
    if (typeof levelIndex == 'undefined') {
      levels = new FileReader("levelsets/defaultlevel.xsb");
      currentIndex = 0;
      fileName = fields.newFileName+".xsb";
    } else {
      levels = new FileReader("levelsets/imported/" + fields.newFileName);
      fileName = fields.newFileName;
      currentIndex = levelIndex-1;
      update=true;
      if (levels.levelsArray[currentIndex]=='undefined') levels.levelsArray.push(levels.levelsArray[currentIndex-1]);
    }

    console.log(levels);
    let customLevel = JSON.parse(fields.levelBoard);
    levels.levelsArray[currentIndex].width = customLevel.width;
    levels.levelsArray[currentIndex].boxCount = customLevel.boxCount;
    levels.levelsArray[currentIndex].height = customLevel.height;
    levels.levelsArray[currentIndex].board = customLevel.board;

    currentIndex++;
    let stringToExport = "";
    levels.levelsArray.forEach((level, index) => {
      let exportFile = level.board.join("").match(new RegExp('.{1,' + level.width + '}', 'g'));
      exportFile.forEach(line => stringToExport += line + "\n");
      stringToExport += ";" + index + "\n"
    })
    fs.closeSync(fs.openSync("levelsets/imported/" + fileName, 'a'));
    fs.writeFileSync("levelsets/imported/" + fileName ,stringToExport,{encoding:'utf8',flag:'w'})
    res.render("levelSetCreated", {
      fileName,
      update
    });
  });
});

app.get(["/root", "/solve", "/upload", "/select"], function(req, res) {
  res.redirect("/");
});

app.get("/downloadFile", function(req, res) {
  const file = "levelsets/imported/" + fileName;
  res.download(file);
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
    res.render("solve", {
      fileName,
      levelCount: levels.levelsArray.length,
      currentlySelected: currentIndex,
      level: levels.levelsArray[0]
    });
  });
});

app.get("/solvePreselec", function(req, res) {
  currentIndex = req.query.currentIndex || 1;
  fileName = req.query.fileSetName;
  if(req.query.custom) levels = new FileReader("levelsets/imported/" + fileName);
  else levels = new FileReader("levelsets/preinstalled/" + fileName);
  res.render("solve", {
    fileName,
    levelCount: levels.levelsArray.length,
    currentlySelected: currentIndex,
    level: levels.levelsArray[currentIndex-1]
  });
});

app.get("/detailsPreselec", function(req, res) {
  let picked = preinstalledLevelSets.find(o => o.fileName === req.query.fileSetName);
  res.render("detailsPreselected", {
    picked
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
  let fileJSON = JSON.parse(fs.readFileSync("solutions/log.json"));
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

const server = app.listen(process.env.port || 3000);

module.exports = server;