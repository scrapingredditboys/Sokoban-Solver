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

module.exports = Level