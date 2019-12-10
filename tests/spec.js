const request = require('supertest');
const jsdom = require("jsdom");
const expect = require('chai').expect;
const { JSDOM } = jsdom;

describe('loading express', () => {
  let server;

  beforeEach(() => {
    delete require.cache[require.resolve('.././index')];
    server = require('.././index');
  });

  afterEach(() => {
    server.close();
  });

  it('displays index on index correctly', (done) => {
    request(server)
      .get('/')
      .end((err,res) => {
        const body = res;
        const dom = new JSDOM(body.text);
        const options = dom.window.document.getElementsByClassName('option');
        expect(options.length).to.equal(4);
        expect(options[0].value).to.equal("Use a solver to solve a level");
        expect(options[1].value).to.equal("View currently existing solutions");
        expect(options[2].value).to.equal("Play Sokoban!");
        expect(options[3].value).to.equal("Edit or create a level (set)");
        done()
      });;
  });

  it('404 pages that don\'t exist', (done) => {
    request(server)
      .get('/soko/ban')
      .expect(404, done);
  });

  describe('testing existing routes', () => {
    const redirectRoutes = ['/root', '/solve', '/upload', '/select'];
    redirectRoutes.forEach(routeName => {
      it(routeName + ' get redirects to index', (done) => {
        request(server)
          .get(routeName)
          .expect(302)
          .expect('Location', '/', done)
        });
    })

    it('displays importFile on /importFile get correctly', (done) => {
      request(server)
        .get('/importFile')
        .end((err,res) => {
          const body = res;
          const dom = new JSDOM(body.text);
          const options = dom.window.document.getElementsByTagName('input');
          expect(options.length).to.equal(2);
          expect(options[0].name).to.equal("fileToUpload");
          expect(options[1].value).to.equal("Submit");
          done()
        });;
    });

    const filePath = __dirname + '/../levelSets/preinstalled/Microban_I.xsb';

    it('imports files and displays /solve page correctly', (done) => {
      request(server)
        .post('/upload')
        .attach('fileToUpload', filePath)
        .end((err, res) => {
          const body = res;
          const dom = new JSDOM(body.text);
          const fileName = dom.window.document.getElementById('fileName');
          expect(fileName.innerHTML).to.equal("Microban_I.xsb");
          const options = dom.window.document.getElementsByTagName('input');
          expect(options[0].max).to.equal('155');
          expect(options[0].value).to.equal('1');
          expect(options[3].value).to.equal('Solve this level!');

          const canvas = dom.window.document.getElementsByTagName('canvas');
          expect(canvas.length).to.equal(1);
          expect(canvas[0].width).to.equal(1280);
          expect(canvas[0].height).to.equal(720);
          done()
        })
    });

    it('displays playSokoban on /playSokoban get correctly', (done) => {
      request(server)
        .get('/playSokoban')
        .end((err,res) => {
          const body = res;
          const dom = new JSDOM(body.text);
          const options = dom.window.document.getElementsByTagName('input');
          expect(options.length).to.equal(2);
          expect(options[0].name).to.equal("fileToUpload");
          expect(options[1].value).to.equal("Submit");
          done()
        });;
    });

    it('displays /playLevel after post correctly', (done) => {
      request(server)
        .post('/playLevel')
        .attach('fileToUpload', filePath)
        .end((err, res) => {
          const body = res;
          const dom = new JSDOM(body.text);
          const fileName = dom.window.document.getElementById('fileName');
          expect(fileName.innerHTML).to.equal("Microban_I.xsb");
          const options = dom.window.document.getElementsByTagName('input');
          expect(options[0].max).to.equal('155');
          expect(options[0].value).to.equal('1');
          expect(options[1].value).to.equal('Select Level');
          expect(options[2].value).to.equal('Start over');

          const canvas = dom.window.document.getElementsByTagName('canvas');
          expect(canvas.length).to.equal(1);
          expect(canvas[0].width).to.equal(1280);
          expect(canvas[0].height).to.equal(720);
          done();
        })
    });

    it('displays different level in /playLevel correctly', (done) => {
      request(server)
        .post('/playLevel')
        .attach('fileToUpload', filePath)
        .end((err, res) => {
          request(server)
          .post('/playOtherLevel')
          .field('selectedLevel', 10)
          .end((err, res) => {
            const body = res;
            const dom = new JSDOM(body.text);
            const fileName = dom.window.document.getElementById('fileName');
            expect(fileName.innerHTML).to.equal("Microban_I.xsb");
            const options = dom.window.document.getElementsByTagName('input');
            expect(options[0].max).to.equal('155');
            expect(options[0].value).to.equal('10');
            done();
        })
      })
    });

    it('gets /playLevel correctly', (done) => {
      request(server)
        .post('/playLevel')
        .attach('fileToUpload', filePath)
        .end((err, res) => {
          request(server)
          .get('/playOtherLevel')
          .end((err, res) => {
            const body = res;
            const dom = new JSDOM(body.text);
            const fileName = dom.window.document.getElementById('fileName');
            expect(fileName.innerHTML).to.equal("Microban_I.xsb");
            const options = dom.window.document.getElementsByTagName('input');
            expect(options[0].max).to.equal('155');
            expect(options[0].value).to.equal('1');
            done();
        })
      })
    });

    it('/playOtherLevel get works correctly', (done) => {
      request(server)
        .post('/playLevel')
        .attach('fileToUpload', filePath)
        .end((err, res) => {
          request(server)
          .get('/playOtherLevel')
          .end((err, res) => {
            const body = res;
            const dom = new JSDOM(body.text);
            const fileName = dom.window.document.getElementById('fileName');
            expect(fileName.innerHTML).to.equal("Microban_I.xsb");
            const options = dom.window.document.getElementsByTagName('input');
            expect(options[0].max).to.equal('155');
            expect(options[0].value).to.equal('1');
            done();
        })
      })
    });


    it('displays solutions on /viewSolutions correctly', (done) => {
      request(server)
        .get('/viewSolutions')
        .end((err, res) => {
          const body = res;
          const dom = new JSDOM(body.text);
          const tableBody = dom.window.document.getElementsByTagName('table');
          expect(tableBody.length).to.equal(1);
          const th = dom.window.document.getElementsByTagName('th');
          expect(th.length).to.equal(8);
          expect(th[0].innerHTML).to.equal("Level ID");
          done()
        })
    });

    it('displays arbitrary solution details on /details?id=0 correctly', (done) => {
      request(server)
        .get('/viewSolutions')
        .end((err, res) => {
          request(server)
            .get('/details?id=0')
            .end((err, res) => {
              const body = res;
              const dom = new JSDOM(body.text);

              const canvas = dom.window.document.getElementsByTagName('canvas');
              expect(canvas.length).to.equal(1);
              expect(canvas[0].width).to.equal(1280);
              expect(canvas[0].height).to.equal(720);

              const inb4info = dom.window.document.getElementById('inb4info');
              expect(inb4info.innerHTML).to.contain("Step: ");

              const details = dom.window.document.getElementById('details');
              expect(details).to.exist;
              done()
            });
          });
        });   
  });
});

