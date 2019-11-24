const express = require('express');
const app = express();
const path = require('path');
const formidable = require('formidable');
const ejs = require('ejs');
const readline = require('readline');
const fs = require('fs');
const router = express.Router();

app.set('view engine', 'ejs');

app.get('/',function(req,res){
    res.render('index');
});

app.get('/importFile',function(req,res){
    res.render('solve', {fileName: '', levelCount: ''});
});

let solutions;

app.get('/viewSolutions',function(req,res){
    solutions = JSON.parse(fs.readFileSync("solutions/log.json"));
    res.render('solutions', {solutions});
});

app.get('/details', function(req, res) {
    res.render('details', {solution: solutions[req.query.id]})
});

app.get('/',function(req,res){
    res.render('index');
});
// Parse URL-encoded bodies (as sent by HTML forms)
app.use(express.urlencoded());
app.use(express.static(__dirname + '/'));

// Parse JSON bodies (as sent by API clients)
app.use(express.json());

class Level {
    constructor(levelData) {
        this.width = this.calculateWidth(levelData);
        this.height = this.calculateHeight(levelData);
        this.boxCount = 0;
    
        this.normalizeLevelData(levelData);
    
        this.board = [];
        for(let i = 0; i < this.height * this.width; i++) {
            this.board[i] = levelData[Math.floor(i / this.width)][Math.floor(i % this.width)];
        }
    }

    print = () => {
        for(let i = 0; i < this.height; i++) {
            for(let j = 0; j < this.width; j++) {
                console.log(board[i * this.width + j]);
            }
            console.log('\n');
        }
    }

    calculateWidth = (levelData) => {
        let maxWidth = levelData[0].length;
        for(let i = 1; i < levelData.length; i++) {
            maxWidth = (maxWidth < levelData[i].length ? levelData[i].length : maxWidth);
        }
        return maxWidth;
    }

    calculateHeight = (levelData) => {
        return levelData.length;
    }

    normalizeLevelData = (levelData) => {
        for(let i = 0; i < levelData.length; i++) {
            for(let j = 0; j < levelData[i].length; j++) {
                if(levelData[i][j] == '-' || levelData[i][j] == '_') {
                    levelData[i][j] = ' ';
                }
                else if(levelData[i][j] == '$' || levelData[i][j] == '*') {
                    this.boxCount++;
                }
            }
            let missingSpaces = this.width - levelData[i].length;
            for(let j = 0; j < missingSpaces; j++) {
                levelData[i] += " ";
            }
        }
    }
}

const BOARD_CHARS = "#@+$*. -_";

class FileReader { 
    constructor(file) {
        this.levelsArray = this.read(file);
    }

    read = (file) => {
        let levelRows = [];
        let inLevel = false;
        let levels = [];

        const data = fs.readFileSync(file, 'UTF-8');

        // split the contents by new line
        const lines = data.split(/\r?\n/);

        lines.forEach((row) => {
            if(this.isBoardRow(row)) {
                inLevel = true;
                levelRows.push(row);
            }
            else {
                if(inLevel) {
                    levels.push(new Level(levelRows));
                    levelRows = [];
                }
                inLevel = false;
            }
        });
        if(inLevel) {
            levels.push(new Level(levelRows));
        }
        return levels;
    }

    isBoardRow = (row) => {
        if(row.length == 0) {
            return false;
        }
    
        let hasOtherThanSpaceChar = false;
        for(let i = 0; i < row.length; i++) {
            let hasBoardChar = false;
            for(let j = 0; j < BOARD_CHARS.length; j++) {
                if(row[i] == BOARD_CHARS[j]) {
                    if(row[i] != ' ') {
                        hasOtherThanSpaceChar = true;
                    }
                    hasBoardChar = true;
                    break;
                }
            }
            if(!hasBoardChar) {
                return false;
            }
        }
        if(!hasOtherThanSpaceChar) {
            return false;
        }
    
        return true;
    }
}

let currentIndex;
let levels = [];
let fileName;
// Access the parse results as request.body
app.post('/upload', function(req, res){
    new formidable.IncomingForm().parse(req, (err, fields, files) => {
        if (err) {
          console.error('Error', err)
          throw err
        }
        currentIndex = 1;
        fileName = files.fileToUpload.name;
        levels = new FileReader(files.fileToUpload.path);
        console.log(levels.levelsArray[0]);
        res.render('solve', { fileName, 
            levelCount: levels.levelsArray.length, 
            currentlySelected: currentIndex, 
            level: levels.levelsArray[0]
        });
      }
    )
});

let selectedMethod = "Kacper";
let arguments = [];

app.post('/select', function(req, res){
    new formidable.IncomingForm().parse(req, (err, fields, files) => {
        if (err) {
          console.error('Error', err)
          throw err
        }
        currentIndex = fields.selectedLevel;
        res.render('solve', { fileName,
            levelCount: levels.levelsArray.length, 
            currentlySelected: currentIndex,
            level: levels.levelsArray[currentIndex-1]
        });
      }
      )
});

let time;

const doingTheSolvingWork = async() => {
    console.log(JSON.parse(fs.readFileSync("solutions/log.json")));
    const fileJSON = JSON.parse(fs.readFileSync("solutions/log.json"));
    let solution = {
        fileName,
        levelIndex: currentIndex,
        level: levels.levelsArray[currentIndex-1],
        method: selectedMethod,
        arguments,
        solution: "DDUUu",
        milisecondsTaken: Date.now() - time,
        startDate: time.toLocaleString('en-GB', { timeZone: 'UTC' }),
    }
    
    fileJSON.push(solution);
    let data = JSON.stringify(fileJSON, null, 2);
    fs.writeFile('solutions/log.json', data, function (err) {
        if (err) throw err;
        console.log('Saving successful');
    });
}


app.post('/solve', function(req, res){
    time = new Date();
    doingTheSolvingWork();
    res.render('scheduledToSolve', {
        currentlySelected: currentIndex, 
    });
});

app.post('/root', function(req, res) {
    res.redirect('/');
});

//add the router
app.use('/', router);
app.listen(process.env.port || 3000);

console.log('Running at Port 3000');