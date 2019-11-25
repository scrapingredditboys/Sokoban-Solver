module.exports = function(app){
    app.use(express.urlencoded());
app.use(express.static(__dirname + '/'));

// Parse JSON bodies (as sent by API clients)
app.use(express.json());
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

    let currentIndex;
    let levels = [];
    let fileName;
    // Access the parse results as request.body
    const doingTheSolvingWork = async(method) => {
        const levelString = levels.levelsArray[currentIndex-1].board.join('').match(new RegExp('.{1,' + levels.levelsArray[currentIndex-1].width + '}', 'g')).join('|');
        let argumentsArray = [];
        let execString;
        switch(method) {
            case 'JSoko':
                execString = 'java -jar solvers\\JSokoSolver.jar "' + levelString + '"';
            case 'Kacper':
                execString = 'solvers\\sokosolver.exe "' + levelString + '"'
        }
        let time = new Date();
        exec(execString, function callback(error, stdout, stderr){
            const fileJSON = JSON.parse(fs.readFileSync("solutions/log.json"));
            let solution = {
                fileName,
                levelIndex: currentIndex,
                level: levels.levelsArray[currentIndex-1],
                method: method,
                arguments: argumentsArray,
                solution: stdout,
                milisecondsTaken: Date.now() - time,
                startDate: time.toLocaleString('en-GB', { timeZone: 'UTC' }),
            }
            
            fileJSON.push(solution);
            let data = JSON.stringify(fileJSON, null, 2);
            fs.writeFile('solutions/log.json', data, function (err) {
                if (err) throw err;
                console.log('Solution stored in the database!');
            });
            return;
        });
    }


    app.post('/solve', function(req, res){
        new formidable.IncomingForm().parse(req, (err, fields, files) => {
            if (err) {
            console.error('Error', err)
            throw err;
            }
            doingTheSolvingWork(fields.method);
            res.render('scheduledToSolve', {
                currentlySelected: currentIndex, 
            });
        }
        )
        
    });

    app.post('/root', function(req, res) {
        res.redirect('/');
    });

    //add the router
    app.use('/', router);
    app.listen(process.env.port || 3000);
    console.log('Running at Port 3000');
}