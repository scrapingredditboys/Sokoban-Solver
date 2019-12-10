const levelSetNames = [
    "Microban I",
    "Microban II",
    "Microban III",
    "Microban IV"
]

const descriptions = [
    "A set from April 2000 created by David W Skinner, the most popular one of the testing suite. According to David, the set is good for beginners and children, features small puzzles and explain certain concepts.",
    "David W Skinner's April 2002 addition to the Microban set. There are four mega puzzles at the end of the set that are hard to solve with solvers that do not feature extended pushes",
    "David W Skinner's December 2009 set created with Aymeric Du Peloux's LOMA project in mind. The puzzles are harder than other Microban sets, but they are kept small.",
    "Alphabet puzzles from August 2010 by David W Skinner. The last addition finished as of the time of writing this application, although a set V is in works ever since 2010."
]

const years = [
    "2000",
    "2002",
    "2009",
    "2010"
]

const levelCounts = [
    155,
    135,
    101,
    102
]

const authors = [
    "David W Skinner",
    "David W Skinner",
    "David W Skinner",
    "David W Skinner"
]

const urls = [
    "http://www.abelmartin.com/rj/sokobanJS/Skinner/David%20W.%20Skinner%20-%20Sokoban.htm",
    "http://www.abelmartin.com/rj/sokobanJS/Skinner/David%20W.%20Skinner%20-%20Sokoban.htm",
    "http://www.abelmartin.com/rj/sokobanJS/Skinner/David%20W.%20Skinner%20-%20Sokoban.htm",
    "http://www.abelmartin.com/rj/sokobanJS/Skinner/David%20W.%20Skinner%20-%20Sokoban.htm",
]

class PreinstalledLevelSet {
    constructor(levelSetName, description, year, levelCount, author, link) {
        this.levelSetName = levelSetName;
        this.fileName = levelSetName.replace(/ /g,"_")+".xsb";
        this.description = description;
        this.year = year;
        this.levelCount = levelCount;
        this.author = author;
        this.link = link;
    }
}

let preinstalledLevelSets = [];

preinstalledSets = () => levelSetNames.forEach((value, index) => {
    let tempLevelSet = new PreinstalledLevelSet(levelSetNames[index], descriptions[index], years[index], levelCounts[index], authors[index], urls[index]);
    preinstalledLevelSets.push(tempLevelSet);
});

preinstalledSets();

module.exports = preinstalledLevelSets;