const fs = require("fs");

const Level = require("./Level.js");

const BOARD_CHARS = "#@+$*. -_";

class FileReader {
  constructor(file) {
    this.levelsArray = this.read(file);
  }

  read = file => {
    let levelRows = [];
    let inLevel = false;
    let levels = [];

    const data = fs.readFileSync(file, "UTF-8");

    // split the contents by new line
    const lines = data.split(/\r?\n/);

    lines.forEach(row => {
      if (this.isBoardRow(row)) {
        inLevel = true;
        levelRows.push(row);
      } else {
        if (inLevel) {
          levels.push(new Level(levelRows));
          levelRows = [];
        }
        inLevel = false;
      }
    });
    if (inLevel) {
      levels.push(new Level(levelRows));
    }
    return levels;
  };

  isBoardRow = row => {
    if (row.length == 0) {
      return false;
    }

    let hasOtherThanSpaceChar = false;
    for (let i = 0; i < row.length; i++) {
      let hasBoardChar = false;
      for (let j = 0; j < BOARD_CHARS.length; j++) {
        if (row[i] == BOARD_CHARS[j]) {
          if (row[i] != " ") {
            hasOtherThanSpaceChar = true;
          }
          hasBoardChar = true;
          break;
        }
      }
      if (!hasBoardChar) {
        return false;
      }
    }
    if (!hasOtherThanSpaceChar) {
      return false;
    }

    return true;
  };
}

module.exports = FileReader;
