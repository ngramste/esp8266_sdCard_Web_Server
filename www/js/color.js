
let sel;
let r;
let g;
let b;
let l;
let mutexLock = false;

function updateAPI (forced) {
  sel = document.getElementById("selectedColor");
  r = document.getElementById("r").value;
  g = document.getElementById("g").value;
  b = document.getElementById("b").value;
  l = document.getElementById("l").value;

  sel.style.backgroundColor = "rgb("+(r*l/100)+","+(g*l/100)+","+(b*l/100)+")";
  if (!mutexLock || forced) {
    mutexLock = true;
    var xhr = new XMLHttpRequest();
    xhr.withCredentials = true;

    xhr.addEventListener("readystatechange", function() {
      if(this.readyState === 4) {
        mutexLock = false;
        console.log(this.responseText);
        let colors = this.responseText.split(",");
        r = colors[0];
        b = colors[1];
        g = colors[2];
        l = colors[3];
      }
    });

    xhr.open("POST", "./api?r="+r+"&g="+g+"&b="+b+"&l="+l);
    xhr.send();
  }
}

function getCurrentColor () {
  var xhr = new XMLHttpRequest();
  xhr.withCredentials = true;

  xhr.addEventListener("readystatechange", function() {
    if(this.readyState === 4) {
      console.log(this.responseText);
      let colors = this.responseText.split(",");
      r = colors[0];
      b = colors[1];
      g = colors[2];
      l = colors[3];

      document.getElementById("r").value = r;
      document.getElementById("g").value = g;
      document.getElementById("b").value = b;
      document.getElementById("l").value = l;
    }
  });

  xhr.open("POST", "./api");
  xhr.send();
}

getCurrentColor();
