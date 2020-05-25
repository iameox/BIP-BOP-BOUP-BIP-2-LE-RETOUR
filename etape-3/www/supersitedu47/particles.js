// ADAPTATIONS POUR LE FAIRE FONCTIONNER SOUS UNE ANCIENNE VERSION DE FIREFOX
// Source: https://github.com/jserz/js_piece/blob/master/DOM/ParentNode/prepend()/prepend().md
(function (arr) {
  arr.forEach(function (item) {
    if (item.hasOwnProperty('prepend')) {
      return;
    }
    Object.defineProperty(item, 'prepend', {
      configurable: true,
      enumerable: true,
      writable: true,
      value: function prepend() {
        var argArr = Array.prototype.slice.call(arguments),
          docFrag = document.createDocumentFragment();
        
        argArr.forEach(function (argItem) {
          var isNode = argItem instanceof Node;
          docFrag.appendChild(isNode ? argItem : document.createTextNode(String(argItem)));
        });
        
        this.insertBefore(docFrag, this.firstChild);
      }
    });
  });
})([Element.prototype, Document.prototype, DocumentFragment.prototype]);



///////////////////////////////////////////////////////////////////////

var canvas = newCanvas(),
    ctx = canvas.getContext('2d'),
    
    maxParticles = 350,
    particles = [];

ctx.lineWidth = 1;
ctx.fillStyle = ctx.strokeStyle = '#34495e';

function $(el) { return document.querySelector(el); }
function rand(max) { return Math.random() * max; }
function randPath(...dirs) { return dirs[Math.floor(rand(dirs.length))] * (1 - Math.random()); }

function dist(x1, y1, x2, y2) {
    var x = x2 - x1,
        y = y2 - y1;
    
    return Math.sqrt(x * x + y * y);
}

function newCanvas() {
    var canvas = document.createElement('canvas');
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;

    $('body').prepend(canvas);

    return canvas;
}

function drawParticles() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    for (var i = maxParticles - 1; i >= 0; i--) {
        var part = particles[i];

        ctx.beginPath();
        ctx.arc(part[0], part[1], 1, 0, 2 * Math.PI);

        ctx.globalAlpha = 1;
        ctx.fill();

        for (var j = maxParticles - 1; j >= 0; j--) {
            var oPart = particles[j],
                distValue = dist(part[0], part[1], oPart[0], oPart[1]);

            if (distValue <= 200 && i !== j && part[6].indexOf(j) === -1) {
                oPart[6].push(i);

                ctx.beginPath();
                ctx.moveTo(part[0], part[1]);
                ctx.lineTo(oPart[0], oPart[1]);

                ctx.globalAlpha = (200 - distValue) / 200;
                ctx.stroke();
            }
        }

        part[6] = [];
        moveParticle(part);
    }
    
    window.requestAnimationFrame(() => drawParticles());
}

function moveParticle(part) {
    if (part[0] < 0 && part[2] >= 0 || part[0] >= canvas.width && part[2] < canvas.width) { part[4] = randPath(-Math.sign(part[4])); }
    if (part[1] < 0 && part[3] >= 0 || part[1] >= canvas.height && part[3] < canvas.height) { part[5] = randPath(-Math.sign(part[5])); }
    
    part[2] = part[0];
    part[3] = part[1];
    part[0] += part[4];
    part[1] += part[5];
}

function pushParticles() {
    for (var i = maxParticles - 1; i >= 0; i--) {
        var coords = [rand(canvas['width']), rand(canvas['height'])],
            path = [randPath(-1, 1), randPath(-1, 1)];

        particles.push(coords.concat(coords, path, [[]]));
    }
    
    drawParticles();
}

pushParticles();